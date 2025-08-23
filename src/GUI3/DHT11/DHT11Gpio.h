// DHT11Gpio.h
#pragma once

#include <gpiod.hpp>
#include <cstdint>
#include <string>

class DHT11Gpio {
public:
    /// @param chipname    "/dev/gpiochip0"
    /// @param line_offset BCM �ţ����� 4��17 ��
    explicit DHT11Gpio(const std::string &chipname,
                       unsigned int line_offset);

    /// �� gpiochip�������ϵ��ȴ� ��1s
    bool begin();

    /// ��æ�ȶ�ȡһ�� DHT11���ɹ����� true
    bool read();

    /// ��ȡ�����read() �ɹ�����ܵ���
    int humidityInt()    const;  ///< ʪ����������
    int humidityDec()    const;  ///< ʪ��С�����֣�DHT11 ʼ�� 0��
    int temperatureInt() const;  ///< �¶���������
    int temperatureDec() const;  ///< �¶�С�����֣�DHT11 ʼ�� 0��

private:
    std::string  chipname_;
    unsigned int line_offset_;
    uint64_t     raw_data_;     // 32 λ���� + 8 λ CRC
    gpiod::chip  chip_;

    /// ������ Epoch �����ڵ�΢����
    static inline uint64_t now_us();

    /// �ȴ� line.value() ��Ϊ target_level����ʱ���� false
    bool waitLevelBusy(gpiod::line &line,
                       int target_level,
                       uint32_t timeout_us);

    /// æ�ȷ�ʽ��ȡ bits λ��sample_delay_us Ϊ�ߵ�ƽ����ʱ�ٶ�����ʱ return false
    bool readBitsBusy(gpiod::line &line,
                      int bits,
                      uint32_t sample_delay_us,
                      uint32_t timeout_us,
                      uint32_t &out);
};
