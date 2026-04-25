/*
This file defines the GpioInputs class, which implements the Inputs interface to read button states from GPIO pins.
*/

#include "Inputs.hpp"
#include <gpiod.hpp>
#include <optional>
#include <stdexcept>
#include <thread>
#include <mutex>

#define PINA 17
#define PINB 27
#define PINX 22
#define PINDIALCLK 25
#define PINDIALDT 23
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
    mutable gpiod::line_request gpioLines;

    mutable bool dialLastClk = false;
    mutable bool dialLastDt = false;
    mutable Dial dialPosition = Dial::NEUTRAL;
    std::thread dialThread;
    mutable bool dialRunning = true;
    mutable std::mutex dialMutex;
};  