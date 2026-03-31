#include "Instrument.hpp"

class Drums : public Instrument {
public:
    Drums();
    virtual stk::StkFloat midiToFrequency(int midiNote) const override;
};