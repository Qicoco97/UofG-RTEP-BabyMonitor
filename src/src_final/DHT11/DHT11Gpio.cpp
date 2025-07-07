// DHT11Gpio.cpp
#include "DHT11Gpio.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono;

inline uint64_t DHT11Gpio::now_us() {
    return duration_cast<microseconds>(
               high_resolution_clock::now().time_since_epoch()
           ).count();
}

DHT11Gpio::DHT11Gpio(const std::string &chipname,
                     unsigned int line_offset)
  : chipname_(chipname)
  , line_offset_(line_offset)
  , raw_data_(0)
  , chip_(chipname_)
{}

bool DHT11Gpio::begin() {
// Check whether the chip can be turned on and wait for the power-on stabilization time to be ≥1s
    try {
        gpiod::chip test(chipname_);
        (void)test.get_line(line_offset_);
    } catch (const std::exception &e) {
        std::cerr << "DHT11Gpio::begin() failed: " << e.what() << "\n";
        return false;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return true;
}

bool DHT11Gpio::waitLevelBusy(gpiod::line &line,
                              int target_level,
                              uint32_t timeout_us)
{
    uint64_t t0 = now_us();
    while (line.get_value() != target_level) {
        if (now_us() - t0 > timeout_us) return false;
    }
    return true;
}

bool DHT11Gpio::readBitsBusy(gpiod::line &line,
                             int bits,
                             uint32_t sample_delay_us,
                             uint32_t timeout_us,
                             uint32_t &out)
{
    out = 0;
    for (int i = 0; i < bits; ++i) {
// Wait for the last high level to end (return high after 50μs initial low pulse)
        if (!waitLevelBusy(line, 1, timeout_us)) {
            std::cerr << "[readBits] bit " << i << " wait HIGH timeout\n";
            return false;
        }
//Wait for high level to start
        if (!waitLevelBusy(line, 1, timeout_us)) {

        }
// Wait for sample_delay_us to read the level
        uint64_t tRise = now_us();
        while (now_us() - tRise < sample_delay_us);
        int val = line.get_value();
        out = (out << 1) | (val ? 1u : 0u);

        if (!waitLevelBusy(line, 0, timeout_us)) {
            std::cerr << "[readBits] bit " << i << " wait FALL timeout\n";
            return false;
        }
    }
    return true;
}

bool DHT11Gpio::read() {
    // 1. 获取一根干净的 line
    gpiod::line line = chip_.get_line(line_offset_);

    // —— Start 信号：拉低 ≥18ms，再拉高 30μs —— 
    line.request({"DHT11", gpiod::line_request::DIRECTION_OUTPUT, 0}, 0);
    line.set_value(0);
    uint64_t t0 = now_us();
    while (now_us() - t0 < 18000);
    line.set_value(1);
    t0 = now_us();
    while (now_us() - t0 < 30);
    line.release();

// —— Handshake: cut input + internal pull-up, wait 80μs↓ + 80μs↑ + 50μs↓ ——
    line.request({"DHT11",
                  gpiod::line_request::DIRECTION_INPUT,
                  gpiod::line_request::FLAG_BIAS_PULL_UP},
                 0);
    if (!waitLevelBusy(line, 0, 1000000)) { line.release(); return false; }
    if (!waitLevelBusy(line, 1, 1000000)) { line.release(); return false; }
    if (!waitLevelBusy(line, 0, 1000000)) { line.release(); return false; }

    // —— 读取 32 位数据 —— 
    uint32_t data = 0;
    if (!readBitsBusy(line, 32, /*sample=*/50, /*timeout=*/2000, data)) {
        line.release(); return false;
    }

    // —— 读取 8 位 CRC —— 
    uint32_t crc = 0;
    if (!readBitsBusy(line, 8, 50, 2000, crc)) {
        line.release(); return false;
    }

    // 校验
    uint8_t b0 = (data >> 24) & 0xFF;
    uint8_t b1 = (data >> 16) & 0xFF;
    uint8_t b2 = (data >>  8) & 0xFF;
    uint8_t b3 = (data >>  0) & 0xFF;
    if (static_cast<uint8_t>(b0 + b1 + b2 + b3) != static_cast<uint8_t>(crc)) {
        line.release(); return false;
    }

    raw_data_ = (static_cast<uint64_t>(data) << 8) | (crc & 0xFF);
    line.release();
    return true;
}

int DHT11Gpio::humidityInt()    const { return (raw_data_ >> 32) & 0xFF; }
int DHT11Gpio::humidityDec()    const { return (raw_data_ >> 24) & 0xFF; }
int DHT11Gpio::temperatureInt() const { return (raw_data_ >> 16) & 0xFF; }
int DHT11Gpio::temperatureDec() const{ return (raw_data_ >>  8) & 0xFF; }
