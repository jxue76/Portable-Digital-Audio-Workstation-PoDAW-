#include "KeyboardInputs.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    try {
        KeyboardInputs keyboard;

        std::cout << "Keyboard test started. Press keys A, S, D, O, P, or arrow keys." << std::endl;
        std::cout << "Test will run for 10 seconds." << std::endl;

        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(10)) {
            // Test keyboard input
            if (keyboard.isUpPressed()) {
                std::cout << "Up arrow is pressed" << std::endl;
            }
            if (keyboard.isDownPressed()) {
                std::cout << "Down arrow is pressed" << std::endl;
            }
            if (keyboard.isLeftPressed()) {
                std::cout << "Left arrow is pressed" << std::endl;
            }
            if (keyboard.isRightPressed()) {
                std::cout << "Right arrow is pressed" << std::endl;
            }
            if (keyboard.isAPressed()) {
                std::cout << "A is pressed" << std::endl;
            }
            if (keyboard.isBPressed()) {
                std::cout << "B (S key) is pressed" << std::endl;
            }
            if (keyboard.isXPressed()) {
                std::cout << "X (D key) is pressed" << std::endl;
            }
            Dial dialPosition = keyboard.getDialPosition();
            if (dialPosition == Dial::UP) {
                std::cout << "Dial is Up (O key)" << std::endl;
            } else if (dialPosition == Dial::DOWN) {
                std::cout << "Dial is Down (P key)" << std::endl;
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