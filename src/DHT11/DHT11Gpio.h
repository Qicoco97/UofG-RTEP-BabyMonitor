// DHT11Gpio.h
#pragma once

#include <gpiod.hpp>
#include <cstdint>
#include <string>

class DHT11Gpio {
public:
    /// @param chipname    "/dev/gpiochip0"
    /// @param line_offset BCM 号，比如 4、17 等
    explicit DHT11Gpio(const std::string &chipname,
                       unsigned int line_offset);

    /// 打开 gpiochip，并在上电后等待 ≥1s
    bool begin();

    /// 用忙等读取一次 DHT11，成功返回 true
    bool read();

    /// 读取结果，read() 成功后才能调用
    int humidityInt()    const;  ///< 湿度整数部分
    int humidityDec()    const;  ///< 湿度小数部分（DHT11 始终 0）
    int temperatureInt() const;  ///< 温度整数部分
    int temperatureDec() const;  ///< 温度小数部分（DHT11 始终 0）

private:
    std::string  chipname_;
    unsigned int line_offset_;
    uint64_t     raw_data_;     // 32 位数据 + 8 位 CRC
    gpiod::chip  chip_;

    /// 返回自 Epoch 起到现在的微秒数
    static inline uint64_t now_us();

    /// 等待 line.value() 变为 target_level，超时返回 false
    bool waitLevelBusy(gpiod::line &line,
                       int target_level,
                       uint32_t timeout_us);

    /// 忙等方式读取 bits 位，sample_delay_us 为高电平后延时再读，超时 return false
    bool readBitsBusy(gpiod::line &line,
                      int bits,
                      uint32_t sample_delay_us,
                      uint32_t timeout_us,
                      uint32_t &out);
};
