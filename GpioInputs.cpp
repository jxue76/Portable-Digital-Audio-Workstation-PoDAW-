#include "GpioInputs.hpp"
#include <gpiod.hpp>
#include <stdexcept>

GpioInputs::GpioInputs() : Inputs(), chip("/dev/gpiochip4"){
    // Initialize GPIO pins
    try {
        ::gpiod::request_builder gpioLineRequest = chip.prepare_request().set_consumer("gpio-inputs");
        for (int pin : pins) {
            gpioLineRequest.add_line_settings(pin, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true));
        }
        gpioLines = gpioLineRequest.do_request();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize GPIO inputs: " + std::string(e.what()));
    }
}

bool GpioInputs::isUpPressed() const {
    return *gpioLines.get_value(PINUP) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isDownPressed() const {
    return *gpioLines.get_value(PINDOWN) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isLeftPressed() const {
    return *gpioLines.get_value(PINLEFT) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isRightPressed() const {
    return *gpioLines.get_value(PINRIGHT) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isAPressed() const {
    return *gpioLines.get_value(PINA) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isBPressed() const {
    return *gpioLines.get_value(PINB) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isXPressed() const {
    return *gpioLines.get_value(PINX) == ::gpiod::line::value::ACTIVE; // Active low
}
Dial GpioInputs::getDialPosition() const {
    bool clk = *gpioLines.get_value(PINDIALCLK) == ::gpiod::line::value::ACTIVE; // Active low
    bool dt = *gpioLines.get_value(PINDIALDT) == ::gpiod::line::value::ACTIVE; // Active low
    if (clk && !dt) {
        return Dial::UP;
    } else if (!clk && dt) {
        return Dial::DOWN;
    } else {
        return Dial::NEUTRAL;
    }
}
