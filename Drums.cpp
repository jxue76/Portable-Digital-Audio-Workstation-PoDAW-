#include "Drums.hpp"
#include "Drummer.h"
#include <cmath>

Drums::Drums()
    : Instrument("Drums", []() { return std::make_unique<stk::Drummer>(); }, 36, 81, 1.0f) {
    tuningOffset = 0.0f; // Drums don't need a tuning offset since they are not pitched instruments, but we can set it to 0 for clarity
}
Drums::Drums(std::string name)
    : Instrument(std::move(name), []() { return std::make_unique<stk::Drummer>(); }, 36, 81, 1.0f) {
    tuningOffset = 0.0f; // Drums don't need a tuning offset since they are not pitched instruments, but we can set it to 0 for clarity
}
void Drums::noteOn(const Note& note) {
    Instrument::noteOn(note);
}
void Drums::noteOff(const Note& note) {
    Instrument::noteOff(note);
}
stk::StkFloat Drums::midiToFrequency(int midiNote) const {
    midiNote = mapBetween(midiNote, midiMin, midiMax);
    return static_cast<stk::StkFloat>(midiNote); // For drums, we can just return the MIDI note number as a float, since each note corresponds to a different drum sound rather than a pitch
}