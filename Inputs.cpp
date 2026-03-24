#include "Inputs.hpp"

Inputs::Inputs() {};
bool Inputs::isUpPressed() const { return false; }
bool Inputs::isDownPressed() const { return false; }
bool Inputs::isLeftPressed() const { return false; }
bool Inputs::isRightPressed() const { return false; }
bool Inputs::isAPressed() const { return false; }
bool Inputs::isBPressed() const { return false; }
bool Inputs::isXPressed() const { return false; }
Dial Inputs::getDialPosition() const { return Dial::NEUTRAL; }