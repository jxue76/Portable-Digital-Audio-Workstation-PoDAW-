#pragma once
#include "Instrument.hpp"

class Piano : public Instrument {
public:
    Piano();
    Piano(std::string name);
    ~Piano() override = default;
};