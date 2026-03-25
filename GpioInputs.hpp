#include "Inputs.hpp"
#include <gpiod.hpp>

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
};