#include "GpioInputs.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    try {
        GpioInputs gpio;

        std::cout << "GPIO test started. Press buttons and rotate the dial." << std::endl;
        std::cout << "Test will run for 10 seconds." << std::endl;

        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(10)) {
            // Test GPIO input
            if (gpio.isUpPressed()) {
                std::cout << "Up button is pressed" << std::endl;
            }
            if (gpio.isDownPressed()) {
                std::cout << "Down button is pressed" << std::endl;
            }
            if (gpio.isLeftPressed()) {
                std::cout << "Left button is pressed" << std::endl;
            }
            if (gpio.isRightPressed()) {
                std::cout << "Right button is pressed" << std::endl;
            }
            if (gpio.isAPressed()) {
                std::cout << "A button is pressed" << std::endl;
            }
            if (gpio.isBPressed()) {
                std::cout << "B button is pressed" << std::endl;
            }
            if (gpio.isXPressed()) {
                std::cout << "X button is pressed" << std::endl;
            }
            Dial dialPosition = gpio.getDialPosition();
            if (dialPosition == Dial::UP) {
                std::cout << "Dial is Up" << std::endl;
            } else if (dialPosition == Dial::DOWN) {
                std::cout << "Dial is Down" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Test completed." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}