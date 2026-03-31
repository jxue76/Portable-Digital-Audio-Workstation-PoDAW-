#include "Drums.hpp"
#include <stk/Drummer.h>

Drums::Drums() : Instrument("Drums", std::make_shared<stk::Drummer>(), 36, 81, 1.0f) {
}

stk::StkFloat Drums::midiToFrequency(int midiNote) const {
    midiNote = mapBetween(midiNote, midiMin, midiMax);
    return static_cast<stk::StkFloat>(midiNote); // For drums, we can just return the MIDI note number as a float, since each note corresponds to a different drum sound rather than a pitch
}