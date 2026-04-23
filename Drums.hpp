#pragma once
#include "Instrument.hpp"

class Drums : public Instrument {
public:
    Drums();
    Drums(std::string name);
    ~Drums() override = default;
    virtual stk::StkFloat midiToFrequency(int midiNote) const override;
};