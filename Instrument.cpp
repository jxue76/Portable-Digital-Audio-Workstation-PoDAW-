#include "Instrument.hpp"

Instrument::Instrument() : name("Generic Instrument"), stkInstrument(nullptr), midiMin(0), midiMax(127), volume(1.0f) {}
Instrument::Instrument(std::string name, std::shared_ptr<stk::Instrmnt> stkInstrument, int midiMin, int midiMax, float volume) : name(name), stkInstrument(stkInstrument), midiMin(midiMin), midiMax(midiMax), volume(volume) {}
Instrument::~Instrument() = default;


std::shared_ptr<stk::Instrmnt> Instrument::getStkInstrument() const {
    if (stkInstrument == nullptr) {
        throw std::runtime_error("STK Instrument not initialized");
    }
    return stkInstrument;
}