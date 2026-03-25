#include "GpioInputs.hpp"
#include <gpiod.hpp>
#include <stdexcept>

GpioInputs::GpioInputs() : Inputs() {
    // Initialize GPIO pins
    try {
        ::gpiod::chip chip("/dev/gpiochip4");
        auto pinA = chip.get_line(17); // GPIO17
        auto pinB = chip.get_line(27); // GPIO27
        auto pinX = chip.get_line(22); // GPIO22
        auto pinUp = chip.get_line(9); // GPIO9
        auto pinDown = chip.get_line(11); // GPIO11
        auto pinLeft = chip.get_line(8); // GPIO8
        auto pinRight = chip.get_line(7); // GPIO7
        auto pinDialClk = chip.get_line(23); // GPIO23
        auto pinDialDt = chip.get_line(24); // GPIO24

        pinA.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinB.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinX.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinUp.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinDown.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinLeft.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinRight.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinDialClk.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
        pinDialDt.request({"gpio_inputs", gpiod::line_request::DIRECTION_INPUT});
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize GPIO inputs: " + std::string(e.what()));
    }
}
bool GpioInputs::isUpPressed() const {
    return pinUp.get_value() == 0; // Active low
}
bool GpioInputs::isDownPressed() const {
    return pinDown.get_value() == 0; // Active low
}
bool GpioInputs::isLeftPressed() const {
    return pinLeft.get_value() == 0; // Active low
}
bool GpioInputs::isRightPressed() const {
    return pinRight.get_value() == 0; // Active low
}
bool GpioInputs::isAPressed() const {
    return pinA.get_value() == 0; // Active low
}
bool GpioInputs::isBPressed() const {
    return pinB.get_value() == 0; // Active low
}
bool GpioInputs::isXPressed() const {
    return pinX.get_value() == 0; // Active low
}
Dial GpioInputs::getDialPosition() const {
    bool clk = pinDialClk.get_value() == 0; // Active low
    bool dt = pinDialDt.get_value() == 0; // Active low
    if (clk && !dt) {
        return Dial::UP;
    } else if (!clk && dt) {
        return Dial::DOWN;
    } else {
        return Dial::NEUTRAL;
    }
}
