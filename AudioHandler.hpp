#pragma once

#include <map>
#include <set>
#include <mutex>

#include "Stk.h"
#include "RtAudio.h"
#include "Instrument.hpp"
#include "Note.hpp"

typedef std::map<std::shared_ptr<Instrument>, std::set<Note>> ActiveNotesMap;
class AudioHandler {
public:
    AudioHandler();
    ~AudioHandler();

    bool addInstrument(std::shared_ptr<Instrument> instrument);
    bool removeInstrument(std::shared_ptr<Instrument> instrument);
    bool addNoteToInstrument(std::shared_ptr<Instrument> instrument, Note note);
    bool removeNoteFromInstrument(std::shared_ptr<Instrument> instrument, Note note);

    inline ActiveNotesMap readActiveNotes() const { // Pass by value to restrict modification of internal state
        std::lock_guard<std::mutex> lock(mtx);
        return activeNotes;
    }

    void printActiveNotes() const; // For debugging purposes

private:
    ActiveNotesMap activeNotes; // Map of active instruments to their currently playing MIDI notes
    RtAudio audio; // RtAudio instance for audio output
    RtAudio::StreamParameters outputParams; // Output parameters for RtAudio
    unsigned int bufferFrames = 256; // Number of frames buffer
    mutable std::mutex mtx; // Mutex for thread-safe access to activeNotes
};