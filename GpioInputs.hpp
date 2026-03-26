//buttons on 17, 27, 22; the rotary encoder on 23, 24; the dpad on 9, 11, 8, 7

#include "Inputs.hpp"
#include <gpiod.hpp>
#include <optional>

#define PINA 17
#define PINB 27
#define PINX 22
#define PINDIALCLK 23
#define PINDIALDT 24
#define PINUP 9
#define PINDOWN 11
#define PINLEFT 8
#define PINRIGHT 7

class GpioInputs : public Inputs {
public:
    GpioInputs();
    ~GpioInputs();
    bool isUpPressed() const override;
    bool isDownPressed() const override;
    bool isLeftPressed() const override;
    bool isRightPressed() const override;
    bool isAPressed() const override;
    bool isBPressed() const override;
    bool isXPressed() const override;
    Dial getDialPosition() const override;
private:
    gpiod::chip chip;
    gpiod::line_request gpioLines;
    int pins[9] = {PINUP, PINDOWN, PINLEFT, PINRIGHT, PINA, PINB, PINX, PINDIALCLK, PINDIALDT};
};