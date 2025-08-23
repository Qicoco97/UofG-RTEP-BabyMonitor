// DHT11Gpio.h
#pragma once

#include <gpiod.hpp>
#include <cstdint>
#include <string>

class DHT11Gpio {
public:
    /// @param chipname    "/dev/gpiochip0" ֮��
    /// @param line_offset BCM ��ţ����� 17
    explicit DHT11Gpio(const std::string &chipname,
                       unsigned int line_offset,
                       uint32_t threshold_us = 1000);

    /// �� gpiochip���ɹ����� true
    bool begin();

    /// ��ȡһ�δ��������ɹ����� true����ͨ�� getter ������
    bool read();

    int humidityInt()    const;  ///< ʪ��������DHT11 ���� 0�C100��
    int humidityDec()    const;  ///< ʪ��С����DHT11 ��Ϊ 0��
    int temperatureInt() const;  ///< �¶�����
    int temperatureDec() const;  ///< �¶�С����DHT11 ��Ϊ 0��

private:
    std::string    chipname_;
    unsigned int   line_offset_;
    uint32_t       threshold_us_;
    uint64_t       raw_data_;    // [31:24] H_int, [23:16] H_dec, [15:8] T_int, [7:0] CRC
    gpiod::chip    chip_;

    // �ȴ� line ��ƽ�� level����ʱ���� false
    bool waitLevel(gpiod::line &line, int level, int timeout_us);

    // �� DHT Э��� bits λ���浽 out
    bool readBits(gpiod::line &line, int bits, uint32_t &out);
};
