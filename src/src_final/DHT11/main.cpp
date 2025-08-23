#include "DHT11Gpio.h"
#include <iostream>
#include <thread>
#include <chrono>


int main() {
    DHT11Gpio sensor("/dev/gpiochip0", /*BCM4=*/17);
    if (!sensor.begin()) {
        std::cerr << "³õÊ¼»¯Ê§°Ü\n";
        return 1;
    }
    while (true) {
        if (sensor.read()) {
            std::cout << "humidity: " << sensor.humidityInt() << "%  "
                      << "temperature: " << sensor.temperatureInt() << "degree\n";
        } else {
            std::cerr << "¶ÁÈ¡Ê§°Ü\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
