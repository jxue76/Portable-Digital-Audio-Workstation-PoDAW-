#include "GpioInputs.hpp"
#include <gpiod.hpp>
#include <stdexcept>

GpioInputs::GpioInputs() : Inputs() {
    // Initialize GPIO pins
    try {
        chip("/dev/gpiochip4");
        lines = chip.prepare_request()
            .set_consumer("gpio-inputs")
            .add_line_settings(PINUP, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINDOWN, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINLEFT, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINRIGHT, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINA, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINB, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINX, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINDIALCLK, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .add_line_settings(PINDIALDT, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
            .do_request();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize GPIO inputs: " + std::string(e.what()));
    }
}

bool GpioInputs::isUpPressed() const {
    return lines.get_value(PINUP) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isDownPressed() const {
    return lines.get_value(PINDOWN) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isLeftPressed() const {
    return lines.get_value(PINLEFT) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isRightPressed() const {
    return lines.get_value(PINRIGHT) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isAPressed() const {
    return lines.get_value(PINA) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isBPressed() const {
    return lines.get_value(PINB) == ::gpiod::line::value::ACTIVE; // Active low
}
bool GpioInputs::isXPressed() const {
    return lines.get_value(PINX) == ::gpiod::line::value::ACTIVE; // Active low
}
Dial GpioInputs::getDialPosition() const {
    bool clk = lines.get_value(PINDIALCLK) == ::gpiod::line::value::ACTIVE; // Active low
    bool dt = lines.get_value(PINDIALDT) == ::gpiod::line::value::ACTIVE; // Active low
    if (clk && !dt) {
        return Dial::UP;
    } else if (!clk && dt) {
        return Dial::DOWN;
    } else {
        return Dial::NEUTRAL;
    }
}
