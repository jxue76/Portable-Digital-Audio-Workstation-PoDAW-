/*
This file contains a simple test program for the GpioInputs class.
Compiled on Raspberry P Zero 2W with:
g++ -std=c++17 -o gpio_test GpioInputs.cpp Inputs.cpp gpio_test.cpp -lgpiodcxx
*/

#include "GpioInputs.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    try {
        int refreshRate = 100; // Default refresh rate in milliseconds
        int testDuration = 10; // Test duration in seconds
        if (argc > 1) {
            try {
                refreshRate = std::stoi(argv[1]);
            } catch (const std::exception& e) {
                std::cerr << "Invalid refresh rate provided. Using default: " << refreshRate << " ms" << std::endl;
            }
        }
        if (argc > 2) {
            try {
                testDuration = std::stoi(argv[2]);
            } catch (const std::exception& e) {
                std::cerr << "Invalid test duration provided. Using default: " << testDuration << " seconds" << std::endl;
            }
        }


        GpioInputs gpio;

        std::cout << "GPIO test started. Press buttons and rotate the dial." << std::endl;
        std::cout << "Test will run for " << testDuration << " seconds." << std::endl;
        std::cout << "Refresh rate: " << refreshRate << " ms" << std::endl;

        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(testDuration)) {
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

            std::this_thread::sleep_for(std::chrono::milliseconds(refreshRate));
        }

        std::cout << "Test completed." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}