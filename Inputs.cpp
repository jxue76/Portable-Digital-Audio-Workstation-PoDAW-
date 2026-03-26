/*
This file defines default behavior for the Inputs class. This behavior is intended to be overriden by all implementations. The default behavior is to return false for all button presses and NEUTRAL for the dial position.
*/

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