// LedController.h
#pragma once

#include <gpiod.h>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <atomic>

/**
* Header version of LEDController,
* Open GPIO when constructed, close and release when destroyed,
* blink() will blink the specified number of times in the background thread.
*/
class LEDController {
public:
    // chipNo: /dev/gpiochipN, lineNo: BCM pin number corresponding line
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
        // Signal any running threads to stop
        blinking_.store(false);

        // Turn off and release resources
        if (line_) {
            gpiod_line_set_value(line_, 0);
            gpiod_line_release(line_);
            line_ = nullptr;  // ✅ 防止悬空指针
        }
        if (chip_) {
            gpiod_chip_close(chip_);
            chip_ = nullptr;  // ✅ 防止悬空指针
        }
    }

    // Asynchronously blink n times, each onMs on, offMs off
    void blink(int n = 3, int onMs = 200, int offMs = 100) {
        // Prevent multiple concurrent blink operations
        if (blinking_.load()) {
            return; // Already blinking, ignore new request
        }

        blinking_.store(true);
        auto *line = line_;
        std::thread([this, line, n, onMs, offMs]{
            for (int i = 0; i < n && line_; ++i) { // Check line_ is still valid
                gpiod_line_set_value(line, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(onMs));
                if (!line_) break; // Exit if object is being destroyed
                gpiod_line_set_value(line, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(offMs));
            }
            blinking_.store(false);
        }).detach();
    }

private:
    gpiod_chip *chip_;
    gpiod_line *line_;
    std::atomic<bool> blinking_{false};
};
