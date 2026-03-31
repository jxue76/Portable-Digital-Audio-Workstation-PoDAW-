class Note {
public:
    Note(int midiNote, stk::StkFloat amplitude) : midiNote(midiNote), amplitude(amplitude) {}
    int getMidiNote() const inline {
        return midiNote;
    }
    stk::StkFloat getAmplitude() const inline {
        return amplitude;
    }
private:
    int midiNote;
    stk::StkFloat amplitude;
};