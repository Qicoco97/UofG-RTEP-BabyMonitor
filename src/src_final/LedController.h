// LedController.h
#pragma once

#include <gpiod.h>
#include <stdexcept>
#include <thread>
#include <chrono>

/**
* Header version of LEDController,
* Open GPIO when constructed, close and release when destroyed,
* blink() will blink the specified number of times in the background thread.
*/
class LEDController {
public:
    // chipNo: /dev/gpiochipN�� lineNo: BCM ���źŶ�Ӧ�� line
    LEDController(int chipNo, int lineNo)
        : chip_(nullptr), line_(nullptr)
    {
        chip_ = gpiod_chip_open_by_number(chipNo);
        if (!chip_) throw std::runtime_error("open LED gpiochip failed");
        line_ = gpiod_chip_get_line(chip_, lineNo);
        if (!line_) {
            gpiod_chip_close(chip_);
            throw std::runtime_error("acquire LED line failed");
        }
        if (gpiod_line_request_output(line_, "LEDCtrl", 0) < 0) {
            gpiod_line_release(line_);
            gpiod_chip_close(chip_);
            throw std::runtime_error("request LED output failed");
        }
    }

    ~LEDController() {
        // Ϩ���ͷ���Դ
        if (line_) {
            gpiod_line_set_value(line_, 0);
            gpiod_line_release(line_);
        }
        if (chip_) gpiod_chip_close(chip_);
    }

    // �첽��˸ n �Σ�ÿ�� onMs ���룬offMs ����
    void blink(int n = 3, int onMs = 200, int offMs = 100) {
        // ������Ҫ��Ա�� lambda
        auto *line = line_;
        std::thread([line, n, onMs, offMs]{
            for (int i = 0; i < n; ++i) {
                gpiod_line_set_value(line, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(onMs));
                gpiod_line_set_value(line, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(offMs));
            }
        }).detach();
    }

private:
    gpiod_chip *chip_;
    gpiod_line *line_;
};
