/**
  FlyKJ - minimal high-performance Mode S / ADS-B receiver for RTL-SDR
 
 Build on Ubuntu / Orange Pi:
    sudo apt install g++ librtlsdr-dev
    g++ -O3 -DNDEBUG -std=c++17 -march=native FlyKJ.cpp \
        -o flykj -lrtlsdr -lpthread -lm
 
  Run (the user must have permission to open the RTL-SDR USB device):
    ./flykj
 
  The program intentionally uses 2.0 MSps as requested.  At that rate each
  0.5 us PPM chip is represented by exactly one magnitude sample.  This is
  computationally cheap, but leaves little timing margin; a higher sampling
  rate plus resampling/phase recovery would improve reception sensitivity.
 */

#include <rtl-sdr.h> 

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr uint32_t kCenterFrequency = 1090000000U;
constexpr uint32_t kSampleRate      = 2000000U;
constexpr std::size_t kPreambleSamples = 16;       // 8 us * 2 samples/us
constexpr std::size_t kMessageBits     = 112;
constexpr std::size_t kFrameSamples = kPreambleSamples + 2 * kMessageBits;
constexpr uint32_t kModeSPolynomial = 0xFFF409U;   // CRC-24 generator, top bit implicit

volatile std::sig_atomic_t g_stop = 0;

void onSignal(int) {
    g_stop = 1;
}

// Extract len (<= 32) bits using the Mode S convention: bit 1 is the MSB of byte 0.
uint32_t getBits(const std::array<uint8_t, 14>& msg, unsigned first, unsigned len) {
    uint32_t value = 0;
    const unsigned zeroBased = first - 1;
    for (unsigned i = 0; i < len; ++i) {
        const unsigned bit = zeroBased + i;
        value = (value << 1U) | ((msg[bit >> 3U] >> (7U - (bit & 7U))) & 1U);
    }
    return value;
}

// A valid 112-bit Mode S codeword has zero remainder.  Checking CRC is vital:
// a preamble-like noise event followed by 112 random PPM decisions is otherwise
// very easy to mistake for an aircraft message.
bool crcOk(const std::array<uint8_t, 14>& msg) {
    uint32_t remainder = 0;
    for (unsigned i = 0; i < 112; ++i) {
        const uint32_t input = (msg[i >> 3U] >> (7U - (i & 7U))) & 1U;
        const uint32_t feedback = ((remainder >> 23U) & 1U) ^ input;
        remainder = (remainder << 1U) & 0xFFFFFFU;
        if (feedback) remainder ^= kModeSPolynomial;
    }
    return remainder == 0;
}

char decodeCallsignChar(uint32_t code) {
    if (code >= 1 && code <= 26) return static_cast<char>('A' + code - 1);
    if (code >= 48 && code <= 57) return static_cast<char>('0' + code - 48);
    if (code == 32) return ' ';
    return ' '; // reserved character
}

std::string decodeCallsign(const std::array<uint8_t, 14>& msg) {
    std::string callsign;
    callsign.reserve(8);
    for (unsigned i = 0; i < 8; ++i)
        callsign.push_back(decodeCallsignChar(getBits(msg, 41 + 6 * i, 6)));

    // ADS-B callsigns are space padded.
    while (!callsign.empty() && callsign.back() == ' ') callsign.pop_back();
    return callsign.empty() ? std::string("(空)") : callsign;
}

struct Velocity {
    bool valid = false;
    bool groundSpeed = false;
    double speedKt = 0.0;
    bool headingValid = false;
    double headingDeg = 0.0;
    bool trueAirspeed = false;
    bool verticalRateValid = false;
    int verticalRateFpm = 0;
};

Velocity decodeVelocity(const std::array<uint8_t, 14>& msg) {
    Velocity v;
    const uint32_t subtype = getBits(msg, 38, 3);
    const double scale = (subtype == 2 || subtype == 4) ? 4.0 : 1.0;

    if (subtype == 1 || subtype == 2) {
        // Subtype 1/2: east-west and north-south ground-speed components.
        const uint32_t ewRaw = getBits(msg, 47, 10);
        const uint32_t nsRaw = getBits(msg, 58, 10);
        if (ewRaw != 0 && nsRaw != 0) {
            double ew = (static_cast<double>(ewRaw) - 1.0) * scale;
            double ns = (static_cast<double>(nsRaw) - 1.0) * scale;
            if (getBits(msg, 46, 1)) ew = -ew; // west is negative
            if (getBits(msg, 57, 1)) ns = -ns; // south is negative
            v.speedKt = std::hypot(ew, ns);
            v.headingDeg = std::atan2(ew, ns) * 180.0 / 3.14159265358979323846;
            if (v.headingDeg < 0.0) v.headingDeg += 360.0;
            v.headingValid = true;
            v.groundSpeed = true;
            v.valid = true;
        }
    } else if (subtype == 3 || subtype == 4) {
        // Subtype 3/4: magnetic heading and indicated/true airspeed.
        const uint32_t airspeedRaw = getBits(msg, 58, 10);
        if (airspeedRaw != 0) {
            v.speedKt = (static_cast<double>(airspeedRaw) - 1.0) * scale;
            v.trueAirspeed = getBits(msg, 57, 1) != 0;
            v.valid = true;
        }
        if (getBits(msg, 46, 1)) {
            v.headingDeg = getBits(msg, 47, 10) * (360.0 / 1024.0);
            v.headingValid = true;
        }
    }

    // Vertical rate is common to all TC=19 subtypes. Unit: 64 ft/min.
    const uint32_t vrRaw = getBits(msg, 70, 9);
    if (vrRaw != 0) {
        int rate = static_cast<int>((vrRaw - 1U) * 64U);
        if (getBits(msg, 69, 1)) rate = -rate;
        v.verticalRateFpm = rate;
        v.verticalRateValid = true;
    }
    return v;
}

struct AircraftState {
    std::string callsign;
};

class AdsbDecoder {
public:
    explicit AdsbDecoder(rtlsdr_dev_t* device) : device_(device) {
        // Center unsigned RTL samples cheaply: abs(0..255 - 127.5), multiplied
        // by two so that no floating point is needed in the hot loop.
        for (unsigned x = 0; x < abs2_.size(); ++x)
            abs2_[x] = static_cast<uint16_t>(std::abs(static_cast<int>(2 * x) - 255));
        stream_.reserve(262144 / 2 + kFrameSamples);
        lastStats_ = std::chrono::steady_clock::now();
    }

    void consume(const uint8_t* iq, uint32_t byteCount) {
        const std::size_t complexSamples = byteCount / 2;
        if (stream_.capacity() < stream_.size() + complexSamples)
            stream_.reserve(stream_.size() + complexSamples);

        uint64_t localMagnitudeSum = 0;
        for (std::size_t n = 0; n < complexSamples; ++n) {
            const uint16_t ai = abs2_[iq[2 * n]];
            const uint16_t aq = abs2_[iq[2 * n + 1]];
            const uint16_t hi = std::max(ai, aq);
            const uint16_t lo = std::min(ai, aq);

            // Approximation to 2*sqrt(I^2+Q^2): max + min/2 is fast and
            // sufficiently monotonic for PPM level comparisons.  Values are
            // still in the doubled coordinate system described above.
            const uint16_t magnitude = static_cast<uint16_t>(hi + (lo >> 1U));
            stream_.push_back(magnitude);
            localMagnitudeSum += magnitude;
        }
        statsSum_ += localMagnitudeSum;
        statsCount_ += complexSamples;

        processAvailableFrames();
        reportStatsIfDue();

        if (g_stop) rtlsdr_cancel_async(device_);
    }

    void cancel() noexcept {
        rtlsdr_cancel_async(device_);
    }

private:
    bool isPreamble(std::size_t p, uint16_t& highAverage, uint16_t& contrast) const {
        const auto m = [this, p](std::size_t i) { return stream_[p + i]; };
        const uint32_t highSum = m(0) + m(2) + m(7) + m(9);
        const uint32_t lowSum = m(1) + m(3) + m(4) + m(5) + m(6) + m(8) +
                                m(10) + m(11) + m(12) + m(13) + m(14) + m(15);
        const uint16_t hi = static_cast<uint16_t>(highSum / 4U);
        const uint16_t lo = static_cast<uint16_t>(lowSum / 12U);

        // Require an adaptive separation rather than a fixed absolute level so
        // weak but clean packets remain decodable as the RF noise floor moves.
        if (hi <= lo || hi - lo < std::max<uint16_t>(10, lo / 3U)) return false;
        const uint16_t threshold = static_cast<uint16_t>((hi + lo) / 2U);

        if (m(0) <= threshold || m(2) <= threshold ||
            m(7) <= threshold || m(9) <= threshold) return false;

        // The long quiet tail is especially useful for rejecting random data
        // transitions that happen to resemble the first four preamble pulses.
        for (std::size_t i : {4U, 5U, 11U, 12U, 13U, 14U, 15U})
            if (m(i) >= threshold) return false;

        if (!(m(0) > m(1) && m(2) > m(3) &&
              m(7) > m(6) && m(7) > m(8) &&
              m(9) > m(8) && m(9) > m(10))) return false;

        highAverage = hi;
        contrast = static_cast<uint16_t>(hi - lo);
        return true;
    }

    bool demodulate(std::size_t p, uint16_t contrast,
                    std::array<uint8_t, 14>& msg, unsigned& weakBits) const {
        msg.fill(0);
        weakBits = 0;
        // Data follows the 8 us preamble.  A Mode S PPM bit occupies 1 us:
        //   bit 1 = [high, low], bit 0 = [low, high].
        // At 2 MSps those two half-bit levels are exactly two samples, so the
        // sign of (first - second) is the Manchester/PPM decision.
        const uint16_t weakThreshold = std::max<uint16_t>(2, contrast / 8U);
        for (std::size_t bit = 0; bit < kMessageBits; ++bit) {
            const uint16_t first  = stream_[p + kPreambleSamples + 2 * bit];
            const uint16_t second = stream_[p + kPreambleSamples + 2 * bit + 1];
            const uint16_t difference = first > second ? first - second : second - first;
            if (difference < weakThreshold) ++weakBits;
            if (first > second)
                msg[bit >> 3U] |= static_cast<uint8_t>(1U << (7U - (bit & 7U)));
        }
        // CRC remains the final authority; this just avoids spending CRC cycles
        // on a very obviously undecodable candidate.
        return weakBits <= 32;
    }

    void processAvailableFrames() {
        std::size_t scan = 0;
        while (scan + kFrameSamples <= stream_.size()) {
            uint16_t highAverage = 0;
            uint16_t contrast = 0;
            if (!isPreamble(scan, highAverage, contrast)) {
                ++scan;
                continue;
            }

            std::array<uint8_t, 14> msg{};
            unsigned weakBits = 0;
            if (!demodulate(scan, contrast, msg, weakBits) ||
                (msg[0] >> 3U) != 17U || !crcOk(msg)) {
                ++scan;
                continue;
            }

            handleDf17(msg, highAverage, weakBits);
            scan += kFrameSamples; // a valid frame cannot contain another frame start
        }

        // Keep only the suffix that might be the beginning of a frame crossing
        // the next librtlsdr callback boundary.
        if (scan != 0) stream_.erase(stream_.begin(), stream_.begin() + scan);
    }

    void handleDf17(const std::array<uint8_t, 14>& msg,
                    uint16_t highAverage, unsigned weakBits) {
        const uint32_t icao = getBits(msg, 9, 24);
        const uint32_t tc = getBits(msg, 33, 5);
        auto& state = aircraft_[icao];

        std::cout << "[ADS-B] ICAO=" << std::uppercase << std::hex
                  << std::setw(6) << std::setfill('0') << icao
                  << std::dec << std::setfill(' ')
                  << " DF=17 TC=" << tc
                  << " 信号=" << highAverage
                  << " 弱判决=" << weakBits;

        if (tc >= 1 && tc <= 4) {
            state.callsign = decodeCallsign(msg);
            std::cout << " 呼号=" << state.callsign;
        } else if (tc == 19) {
            if (!state.callsign.empty()) std::cout << " 呼号=" << state.callsign;
            const Velocity v = decodeVelocity(msg);
            if (v.valid) {
                std::cout << std::fixed << std::setprecision(1)
                          << (v.groundSpeed ? " 地速=" : (v.trueAirspeed ? " 真空速=" : " 指示空速="))
                          << v.speedKt << "kt";
            } else {
                std::cout << " 速度=N/A";
            }
            if (v.headingValid)
                std::cout << std::fixed << std::setprecision(1)
                          << " 航向=" << v.headingDeg << "deg";
            if (v.verticalRateValid)
                std::cout << " 垂直率=" << v.verticalRateFpm << "ft/min";
        }
        std::cout << '\n';
    }

    void reportStatsIfDue() {
        const auto now = std::chrono::steady_clock::now();
        if (now - lastStats_ < std::chrono::seconds(1)) return;
        if (statsCount_ != 0) {
            const double average = static_cast<double>(statsSum_) / statsCount_;
            // With the chosen doubled max+min/2 approximation the full-scale
            // corner is about 382.5. This is an indicative dBFS, not calibrated RSSI.
            const double dbfs = 20.0 * std::log10(std::max(average, 1.0) / 382.5);
            std::cout << std::fixed << std::setprecision(1)
                      << "[信号] 平均幅度=" << average
                      << " 约=" << dbfs << " dBFS"
                      << " 缓冲样本=" << stream_.size() << '\n';
        }
        statsSum_ = 0;
        statsCount_ = 0;
        lastStats_ = now;
    }

    rtlsdr_dev_t* device_;
    std::array<uint16_t, 256> abs2_{};
    std::vector<uint16_t> stream_;
    std::unordered_map<uint32_t, AircraftState> aircraft_;
    uint64_t statsSum_ = 0;
    uint64_t statsCount_ = 0;
    std::chrono::steady_clock::time_point lastStats_;
};

void rtlsdr_cb(unsigned char* buf, uint32_t len, void* context) {
    if (context == nullptr || buf == nullptr || len < 2) return;
    auto* decoder = static_cast<AdsbDecoder*>(context);
    try {
        decoder->consume(buf, len & ~1U);
    } catch (const std::exception& e) {
        std::cerr << "[错误] SDR 回调处理失败: " << e.what() << '\n';
        g_stop = 1;
        decoder->cancel();
    } catch (...) {
        std::cerr << "[错误] SDR 回调发生未知异常\n";
        g_stop = 1;
        decoder->cancel();
    }
}

void requireRtl(int status, const char* operation) {
    if (status < 0)
        throw std::runtime_error(std::string(operation) + " 失败，错误码 " +
                                 std::to_string(status));
}

} // namespace

int main(int argc, char** argv) {
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    uint32_t deviceIndex = 0;
    if (argc > 2) {
        std::cerr << "用法: " << argv[0] << " [RTL-SDR设备序号]\n";
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        char* end = nullptr;
        const unsigned long parsed = std::strtoul(argv[1], &end, 10);
        if (end == argv[1] || *end != '\0' || parsed > std::numeric_limits<uint32_t>::max()) {
            std::cerr << "无效的设备序号: " << argv[1] << '\n';
            return EXIT_FAILURE;
        }
        deviceIndex = static_cast<uint32_t>(parsed);
    }

    const uint32_t count = rtlsdr_get_device_count();
    std::cout << "[初始化] 找到 " << count << " 个 RTL-SDR 设备\n";
    if (deviceIndex >= count) {
        std::cerr << "[错误] RTL-SDR 设备序号不存在\n";
        return EXIT_FAILURE;
    }

    char manufacturer[256]{};
    char product[256]{};
    char serial[256]{};
    if (rtlsdr_get_device_usb_strings(deviceIndex, manufacturer, product, serial) == 0)
        std::cout << "[初始化] 打开设备 " << deviceIndex << ": "
                  << manufacturer << " / " << product << " / " << serial << '\n';

    rtlsdr_dev_t* device = nullptr;
    try {
        requireRtl(rtlsdr_open(&device, deviceIndex), "打开 RTL-SDR");
        requireRtl(rtlsdr_set_center_freq(device, kCenterFrequency), "设置中心频率");
        requireRtl(rtlsdr_set_sample_rate(device, kSampleRate), "设置采样率");

        /**
         * rtlsdr_set_tuner_gain_mode 参数说明：
         * 0 = 调谐器自动增益模式（由芯片自动控制前端LNA增益）
         * 1 = 调谐器手动增益模式，启用后才可调用 rtlsdr_set_tuner_gain() 设置固定dB增益
         * 下方 rtlsdr_set_agc_mode 是 RTL2832U 后端ADC数字AGC，和调谐器增益模式相互独立
         * 本地射频信号强时，建议切换为手动固定调谐增益，避免信号饱和失真
         */
       
        requireRtl(rtlsdr_set_tuner_gain_mode(device, 0), "开启调谐器自动增益"); 
        requireRtl(rtlsdr_set_agc_mode(device, 1), "开启 RTL2832U AGC"); 
        requireRtl(rtlsdr_reset_buffer(device), "复位 USB 缓冲区"); 

        std::cout << "[初始化] 中心频率=" << rtlsdr_get_center_freq(device)
                  << " Hz, 采样率=" << rtlsdr_get_sample_rate(device)
                  << " S/s, AGC=开启\n";
        std::cout << "[运行] 正在异步接收；按 Ctrl+C 停止。\n";

        AdsbDecoder decoder(device);
        // buf_num=0 and buf_len=0 ask librtlsdr to use its efficient defaults.
        const int status = rtlsdr_read_async(device, rtlsdr_cb, &decoder, 0, 0);
        if (status < 0 && !g_stop)
            throw std::runtime_error("异步读取失败，错误码 " + std::to_string(status));

        rtlsdr_close(device);
        device = nullptr;
        std::cout << "[结束] 接收已停止，设备已关闭。\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "[错误] " << e.what() << '\n';
        if (device != nullptr) rtlsdr_close(device);
        return EXIT_FAILURE;
    }
}
