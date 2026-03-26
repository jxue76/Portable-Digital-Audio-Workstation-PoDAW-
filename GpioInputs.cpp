#include "GpioInputs.hpp"
#include <gpiod.hpp>
#include <stdexcept>

GpioInputs::GpioInputs() : 
    Inputs(), 
    chip("/dev/gpiochip4"),
    gpioLines(chip.prepare_request()
        .set_consumer("gpio-inputs")
        .add_line_settings(PINUP, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINDOWN, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINLEFT, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINRIGHT, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINA, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINB, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINX, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINDIALCLK, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true))
        .add_line_settings(PINDIALDT, ::gpiod::line_settings().set_direction(::gpiod::line::direction::INPUT).set_active_low(true)).do_request())
{
    if (!gpioLines) {
        throw std::runtime_error("Failed to request GPIO lines");
    }
}

bool GpioInputs::isUpPressed() const {
    bool rtn = gpioLines.get_value(PINUP) == ::gpiod::line::value::ACTIVE; // Active low
    return rtn;
}
bool GpioInputs::isDownPressed() const {
    bool rtn = gpioLines.get_value(PINDOWN) == ::gpiod::line::value::ACTIVE; // Active low
    return rtn;
}
bool GpioInputs::isLeftPressed() const {
    bool rtn = gpioLines.get_value(PINLEFT) == ::gpiod::line::value::ACTIVE; // Active low
    return rtn;
}
bool GpioInputs::isRightPressed() const {
    bool rtn = gpioLines.get_value(PINRIGHT) == ::gpiod::line::value::ACTIVE; // Active low
    return rtn;
}
bool GpioInputs::isAPressed() const {
    bool rtn = gpioLines.get_value(PINA) == ::gpiod::line::value::ACTIVE; // Active low
    return rtn;
}
bool GpioInputs::isBPressed() const {
    bool rtn = gpioLines.get_value(PINB) == ::gpiod::line::value::ACTIVE; // Active low
    return rtn;
}
bool GpioInputs::isXPressed() const {
    bool rtn = gpioLines.get_value(PINX) == ::gpiod::line::value::ACTIVE; // Active low
    return rtn;
}
Dial GpioInputs::getDialPosition() const {
    bool clk = gpioLines.get_value(PINDIALCLK) == ::gpiod::line::value::ACTIVE; // Active low
    bool dt = gpioLines.get_value(PINDIALDT) == ::gpiod::line::value::ACTIVE; // Active low

    int prevState = (static_cast<int>(dialLastClk) << 1) | static_cast<int>(dialLastDt);
    int currState = (static_cast<int>(clk) << 1) | static_cast<int>(dt);

    Dial dial = Dial::NEUTRAL;
    if (currState != prevState) {
        // Valid clockwise sequence: 00 -> 01 -> 11 -> 10 -> 00
        if ((prevState == 0 && currState == 1) ||
            (prevState == 1 && currState == 3) ||
            (prevState == 3 && currState == 2) ||
            (prevState == 2 && currState == 0)) {
            dial = Dial::UP;
        }
        // Valid counterclockwise sequence: 00 -> 10 -> 11 -> 01 -> 00
        else if ((prevState == 0 && currState == 2) ||
                 (prevState == 2 && currState == 3) ||
                 (prevState == 3 && currState == 1) ||
                 (prevState == 1 && currState == 0)) {
            dial = Dial::DOWN;
        }
    }

    dialLastClk = clk;
    dialLastDt = dt;
    return dial;
}
