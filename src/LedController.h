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

        // Wait for any running blink thread to complete
        if (blinkThread_.joinable()) {
            blinkThread_.join();
        }

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

        // Wait for any previous thread to complete
        if (blinkThread_.joinable()) {
            blinkThread_.join();
        }

        blinking_.store(true);
        blinkThread_ = std::thread([this, n, onMs, offMs]{
            for (int i = 0; i < n && blinking_.load(); ++i) {
                // Check if object is still valid and resources available
                if (!line_ || !chip_) break;

                gpiod_line_set_value(line_, 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(onMs));

                // Check again before turning off
                if (!blinking_.load() || !line_ || !chip_) break;

                gpiod_line_set_value(line_, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(offMs));
            }
            blinking_.store(false);
        });
    }

private:
    gpiod_chip *chip_;
    gpiod_line *line_;
    std::atomic<bool> blinking_{false};
    std::thread blinkThread_;  // Store thread to allow proper cleanup
};
