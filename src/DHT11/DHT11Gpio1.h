// DHT11Gpio.h
#pragma once

#include <gpiod.hpp>
#include <cstdint>
#include <string>

class DHT11Gpio {
public:
    /// @param chipname    "/dev/gpiochip0" 之类
    /// @param line_offset BCM 编号，比如 17
    explicit DHT11Gpio(const std::string &chipname,
                       unsigned int line_offset,
                       uint32_t threshold_us = 1000);

    /// 打开 gpiochip，成功返回 true
    bool begin();

    /// 读取一次传感器，成功返回 true，可通过 getter 拿数据
    bool read();

    int humidityInt()    const;  ///< 湿度整数（DHT11 总是 0–100）
    int humidityDec()    const;  ///< 湿度小数（DHT11 恒为 0）
    int temperatureInt() const;  ///< 温度整数
    int temperatureDec() const;  ///< 温度小数（DHT11 恒为 0）

private:
    std::string    chipname_;
    unsigned int   line_offset_;
    uint32_t       threshold_us_;
    uint64_t       raw_data_;    // [31:24] H_int, [23:16] H_dec, [15:8] T_int, [7:0] CRC
    gpiod::chip    chip_;

    // 等待 line 电平到 level，超时返回 false
    bool waitLevel(gpiod::line &line, int level, int timeout_us);

    // 按 DHT 协议读 bits 位，存到 out
    bool readBits(gpiod::line &line, int bits, uint32_t &out);
};
