#include "Guitar.hpp"
#include "Plucked.h"

Guitar::Guitar()
    : Instrument("Guitar", []() { return std::make_unique<stk::Plucked>(); }, 40, 88, 1.0f) {
}
Guitar::Guitar(std::string name)
    : Instrument(std::move(name), []() { return std::make_unique<stk::Plucked>(); }, 40, 88, 1.0f) {
}
