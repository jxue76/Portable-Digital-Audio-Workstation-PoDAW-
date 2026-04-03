#pragma once
#include <stk/Stk.h>

class Note {
public:
    Note(int midiNote, stk::StkFloat amplitude) : midiNote(midiNote), amplitude(amplitude) {}
    inline int getMidiNote() const {
        return midiNote;
    }
    inline stk::StkFloat getAmplitude() const {
        return amplitude;
    }
    inline bool operator<(const Note& other) const {
        return midiNote < other.midiNote; // Compare based on MIDI note number for ordering in sets
    }
private:
    int midiNote;
    stk::StkFloat amplitude;
};