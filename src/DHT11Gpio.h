// DHT11Gpio.h
#pragma once

#include <gpiod.hpp>
#include <cstdint>
#include <string>

class DHT11Gpio {
public:
    /// @param chipname    "/dev/gpiochip0"
    /// @param line_offset BCM pin number
    explicit DHT11Gpio(const std::string &chipname,
                       unsigned int line_offset);

    /// Open gpiochip, initialize connection and wait about 1s
    bool begin();

    /// Blocking read from DHT11 once, return true on success
    bool read();

    /// Get data from last successful read()
    int humidityInt()    const;  ///< Humidity integer part
    int humidityDec()    const;  ///< Humidity decimal part (always 0 for DHT11)
    int temperatureInt() const;  ///< Temperature integer part
    int temperatureDec() const;  ///< Temperature decimal part (always 0 for DHT11)

private:
    std::string  chipname_;
    unsigned int line_offset_;
    uint64_t     raw_data_;     // 32 bit data + 8 bit CRC
    gpiod::chip  chip_;

    /// Get current time in microseconds since Epoch
    static inline uint64_t now_us();

    /// Wait for line.value() to become target_level, return false on timeout
    bool waitLevelBusy(gpiod::line &line,
                       int target_level,
                       uint32_t timeout_us);

    /// Busy-wait read bits, sample_delay_us for high level sampling delay, return false on timeout
    bool readBitsBusy(gpiod::line &line,
                      int bits,
                      uint32_t sample_delay_us,
                      uint32_t timeout_us,
                      uint32_t &out);
};
