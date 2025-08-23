// LedController.h
#pragma once

#include <gpiod.h>
#include <stdexcept>
#include <thread>
#include <chrono>

/**
 * 头文件版 LEDController，
 * 构造时打开 GPIO，析构时关闭并释放，
 * blink() 会在后台线程闪烁指定次数。
 */
class LEDController {
public:
    // chipNo: /dev/gpiochipN， lineNo: BCM 引脚号对应的 line
    LEDController(int chipNo, int lineNo)
        : chip_(nullptr), line_(nullptr)
    {
        chip_ = gpiod_chip_open_by_number(chipNo);
        if (!chip_) throw std::runtime_error("打开 LED gpiochip 失败");
        line_ = gpiod_chip_get_line(chip_, lineNo);
        if (!line_) {
            gpiod_chip_close(chip_);
            throw std::runtime_error("获取 LED line 失败");
        }
        if (gpiod_line_request_output(line_, "LEDCtrl", 0) < 0) {
            gpiod_line_release(line_);
            gpiod_chip_close(chip_);
            throw std::runtime_error("请求 LED 输出失败");
        }
    }

    ~LEDController() {
        // 熄灭并释放资源
        if (line_) {
            gpiod_line_set_value(line_, 0);
            gpiod_line_release(line_);
        }
        if (chip_) gpiod_chip_close(chip_);
    }

    // 异步闪烁 n 次，每次 onMs 毫秒，offMs 毫秒
    void blink(int n = 3, int onMs = 200, int offMs = 100) {
        // 拷贝必要成员到 lambda
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
