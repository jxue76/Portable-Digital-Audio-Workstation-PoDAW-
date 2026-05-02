#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include <cstdint>
#include <cstdio>
#include "Inputs.hpp"

struct MidiNote {
    uint8_t note     = 60;
    uint8_t velocity = 100;
    uint8_t channel  = 0;
};

class Sequencer {
public:
    static constexpr int NUM_TRACKS = 4;
    static constexpr int NUM_STEPS  = 16;

    enum class Mode { STOPPED, PLAYING, RECORDING };

    struct Step {
        MidiNote note = {};
        bool active = false;
    };

    struct Track {
        char    name[16] = "Track";
        Step    steps[NUM_STEPS] = {};
        uint8_t midiChannel = 0;
    };

    Sequencer();

    void setStepNote(int track, int step, uint8_t noteNum, uint8_t vel);
    void clearStep(int track, int step);
    void clearTrack(int track);
    bool hasNote(int track, int step) const;
    const MidiNote& getNote(int track, int step) const;

    void play();
    void stop();
    void togglePlayback();
    void toggleRecording();
    void advanceStep();
    void resetPlayhead();
    void movePlayhead(int delta);
    void setArmedTrack(int track);
    void recordMidiEvent(const MidiInputEvent& event);

    Mode getMode() const { return mode; }
    bool isStopped() const {
        if (mode == Mode::STOPPED) return true;
        else return false;
    }
    int getCurrentStep() const { return currentStep; }
    int getArmedTrack() const { return armedTrack; }
    bool hasLiveInputNote() const { return liveInputActive; }
    uint8_t getLiveInputNote() const { return liveInputNote; }
    uint8_t getLastRecordedNote() const { return lastRecordedNote; }

    int tempo = 120;
    int timeSigNum = 4;
    int timeSigDen = 4;
    bool looping = true;
    bool metronome = false;
    uint8_t inputVelocity = 100;

    Track tracks[NUM_TRACKS];
    int currentTrack = 1;
    int currentMode = 0;

    static const char* noteToName(uint8_t note);
    
private:
    Mode mode = Mode::STOPPED;
    int currentStep = 0;
    int armedTrack = 0;
    bool liveInputActive = false;
    uint8_t liveInputNote = 60;
    uint8_t lastRecordedNote = 60;
};

#endif
