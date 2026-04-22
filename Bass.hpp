#pragma once
#include "Instrument.hpp"

class Bass : public Instrument {
public:
    Bass();
    Bass(std::string name);
    ~Bass() override = default;
};