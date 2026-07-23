#include "power.h"

uint32_t power_calc_sample(IQSample sample) {
    int16_t i_s = (int16_t)sample.i - 128;
    int16_t q_s = (int16_t)sample.q - 128;
  
    // 强制转换为 int32_t 进行乘法运算，规避未来扩展 16-bit ADC 时的溢出隐患
    //Force cast to int32_t for multiplication to avoid overflow issues when expanding to a 16-bit ADC in the future.
    return (uint32_t)((int32_t)i_s * i_s + (int32_t)q_s * q_s);
  
}

void power_calc_buffer(const IQSample *in_samples, uint32_t *out_power, size_t count) {
    if (!in_samples || !out_power) return;

    for (size_t i = 0; i < count; i++) {
        int16_t i_s = (int16_t)in_samples[i].i - 128;
        int16_t q_s = (int16_t)in_samples[i].q - 128;
        out_power[i] = (uint32_t)((int32_t)i_s * i_s + (int32_t)q_s * q_s);
    }
}
