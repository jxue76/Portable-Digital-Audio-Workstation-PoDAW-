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
private:
    int midiNote;
    stk::StkFloat amplitude;
};