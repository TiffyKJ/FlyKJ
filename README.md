# FlyKJ
High-performance C++ Mode S/ADS-B receiver for RTL-SDR on ARM64 Linux, featuring direct IQ demodulation, PPM decoding, CRC validation, and aircraft data parsing—without dump1090.
基于 RTL-SDR 和 C++ 的高性能 Mode S/ADS-B 接收解码器，支持 IQ 解调、PPM 解码、CRC 校验及飞机信息解析，无需 dump1090。

                1090 MHz ADS-B Receiver

RF Layer
---------
RTL-SDR
   |
   v
IQ Samples


DSP Layer
---------
Power Detection
Noise Estimation
Preamble Synchronization


Demodulation Layer
------------------
PPM Decoder
CRC Validation


Protocol Layer
--------------
DF17 Parser


Application Layer
-----------------
TC Dispatcher

 |----------|----------|
Callsign   Position   Velocity

                |
                v

        Aircraft State Database
