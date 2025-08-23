// DHT11Gpio.cpp
#include "DHT11Gpio.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono;

DHT11Gpio::DHT11Gpio(const std::string &chipname,
                     unsigned int line_offset,
                     uint32_t threshold_us)
  : chipname_(chipname)
  , line_offset_(line_offset)
  , threshold_us_(threshold_us)
  , raw_data_(0)
  , chip_(chipname_)    // 打开 /dev/gpiochipX
{}

bool DHT11Gpio::begin() {
    // 测试能否打开芯片
    try {
        // 构造一个暂时的 line 实例看能否获取
        gpiod::line line = chip_.get_line(line_offset_);
        (void)line;
    } catch (const std::exception &e) {
        std::cerr << "DHT11Gpio::begin() 打开失败: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool DHT11Gpio::waitLevel(gpiod::line &line, int level, int timeout_us) {
    auto start = high_resolution_clock::now();
    while (true) {
        if (line.get_value() == level) return true;
        if (duration_cast<microseconds>(high_resolution_clock::now() - start)
                .count() > timeout_us)
            return false;
    }
}

/*bool DHT11Gpio::readBits(gpiod::line &line, int bits, uint32_t &out) {
    out = 0;
    for (int i = 0; i < bits; ++i) {
        // 等待低→高
        if (!waitLevel(line, 0, 100)) return false;
        if (!waitLevel(line, 1, 100)) return false;
        // 开始计时高电平
        auto t1 = high_resolution_clock::now();
        if (!waitLevel(line, 0, 100)) return false;
        auto t2 = high_resolution_clock::now();
        uint64_t pulse = duration_cast<microseconds>(t2 - t1).count();
        out <<= 1;
        if (pulse > threshold_us_) out |= 1;
    }
    return true;
}*/

bool DHT11Gpio::readBits(gpiod::line &line, int bits, uint32_t &out) {
    out = 0;
    for (int i = 0; i < bits; ++i) {
        // 等待低电平（开始位）
        if (!waitLevel(line, 0, 200)) {
            std::cerr << "[readBits] bit " << i << " wait LOW timeout\n";
            return false;
        }
        // 等待高电平（测高脉冲开始）
        if (!waitLevel(line, 1, 200)) {
            std::cerr << "[readBits] bit " << i << " wait HIGH timeout\n";
            return false;
        }

        // 计时：从高电平开始，到低电平回来
        auto t1 = high_resolution_clock::now();
        if (!waitLevel(line, 0, 200)) {
            std::cerr << "[readBits] bit " << i << " wait FALL timeout\n";
            return false;
        }
        auto t2 = high_resolution_clock::now();

        uint64_t pulse = duration_cast<microseconds>(t2 - t1).count();
        std::cerr << "[readBits] bit " << i << " pulse_us=" << pulse << "\n";

        out <<= 1;
        // 如果高电平宽度 > 阈值 就当作 1
        if (pulse > threshold_us_) {
            out |= 1;
            std::cerr << "[readBits] bit " << i << " = 1\n";
        } else {
            std::cerr << "[readBits] bit " << i << " = 0\n";
        }
    }
    return true;
}


bool DHT11Gpio::read() {
    // 每次都重新拿一根 line，避免资源冲突
    gpiod::line line = chip_.get_line(line_offset_);
    std::cout << "open" << std::endl;

    // —— 1. 输出拉低 ≥18ms —— 
    line.request({"DHT11", gpiod::line_request::DIRECTION_OUTPUT, 0}, 0);
    std::cout << "open1" << std::endl;
    line.set_value(0);
    std::cout << "open2" << std::endl;
    std::this_thread::sleep_for(milliseconds(18));
    std::cout << "open3" << std::endl;
    // —— 拉高 30μs —— 
    line.set_value(1);
    std::this_thread::sleep_for(microseconds(30));
    line.release();

    // —— 2. 切输入，等待响应 80μs↓ + 80μs↑ + 50μs↓ —— 
    line.request({"DHT11",
              gpiod::line_request::DIRECTION_INPUT,
              gpiod::line_request::FLAG_BIAS_PULL_UP},
             0);
    if (!waitLevel(line, 0, 100) ||
        !waitLevel(line, 1, 100) ||
        !waitLevel(line, 0, 100))
    {std::cout << "open4" << std::endl;
        line.release();
        return false;
    }

    // —— 3. 读 32 位数据 —— 
    uint32_t data = 0;
    if (!readBits(line, 32, data)) {
        line.release();
        std::cout << "open5" << std::endl;
        return false;
    }

    // —— 4. 读 8 位 CRC —— 
    uint32_t crc = 0;
    if (!readBits(line, 8, crc)) {
    std::cout << "open6" << std::endl;
        line.release();
        return false;
    }

    // —— 5. 校验并存 raw_data_ —— 
    uint8_t b0 = (data >> 24) & 0xFF;
    uint8_t b1 = (data >> 16) & 0xFF;
    uint8_t b2 = (data >>  8) & 0xFF;
    uint8_t b3 = (data >>  0) & 0xFF;
    if (static_cast<uint8_t>(b0 + b1 + b2 + b3) != static_cast<uint8_t>(crc)) {
    std::cout << "open7" << std::endl;
        line.release();
        return false;
    }
    raw_data_ = (static_cast<uint64_t>(data) << 8) | (crc & 0xFF);

    line.release();
    return true;
}

int DHT11Gpio::humidityInt()    const { return (raw_data_ >> 32) & 0xFF; }
int DHT11Gpio::humidityDec()    const { return (raw_data_ >> 24) & 0xFF; }
int DHT11Gpio::temperatureInt() const { return (raw_data_ >> 16) & 0xFF; }
int DHT11Gpio::temperatureDec() const{ return (raw_data_ >>  8) & 0xFF; }
