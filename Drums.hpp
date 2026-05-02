#pragma once
#include "Instrument.hpp"

class Drums : public Instrument {
public:
    Drums();
    Drums(std::string name);
    ~Drums() override = default;
    virtual void noteOn(const Note& note) override;
    virtual void noteOff(const Note& note) override;
    virtual stk::StkFloat midiToFrequency(int midiNote) const override;
};