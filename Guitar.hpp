#pragma once
#include "Instrument.hpp"

class Guitar : public Instrument {
public:
    Guitar();
    Guitar(std::string name);
    ~Guitar() override = default;
};