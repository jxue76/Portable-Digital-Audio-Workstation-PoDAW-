#include "Sequencer.hpp"

Sequencer::Sequencer() {
    for (int i = 0; i < NUM_TRACKS; i++) {
        snprintf(tracks[i].name, sizeof(tracks[i].name), "Track %d", i + 1);
        tracks[i].midiChannel = static_cast<uint8_t>(i);
    }
}

void Sequencer::setStepNote(int track, int step, uint8_t noteNum, uint8_t vel) {
    if (track < 0 || track >= NUM_TRACKS || step < 0 || step >= NUM_STEPS) return;
    tracks[track].steps[step].active = true;
    tracks[track].steps[step].note.note = noteNum;
    tracks[track].steps[step].note.velocity = vel;
    tracks[track].steps[step].note.channel = tracks[track].midiChannel;
    lastRecordedNote = noteNum;
}

void Sequencer::clearStep(int track, int step) {
    if (track < 0 || track >= NUM_TRACKS || step < 0 || step >= NUM_STEPS) return;
    tracks[track].steps[step].active = false;
}

void Sequencer::clearTrack(int track) {
    if (track < 0 || track >= NUM_TRACKS) return;
    for (int step = 0; step < NUM_STEPS; ++step) {
        tracks[track].steps[step].active = false;
    }
}

bool Sequencer::hasNote(int track, int step) const {
    if (track < 0 || track >= NUM_TRACKS || step < 0 || step >= NUM_STEPS) return false;
    return tracks[track].steps[step].active;
}

const MidiNote& Sequencer::getNote(int track, int step) const {
    return tracks[track].steps[step].note;
}

void Sequencer::play() {
    mode = Mode::PLAYING;
}

void Sequencer::stop() {
    mode = Mode::STOPPED;
}

void Sequencer::togglePlayback() {
    if (mode == Mode::STOPPED) {
        play();
    } else {
        stop();
    }
}

void Sequencer::toggleRecording() {
    if (mode == Mode::RECORDING) {
        mode = Mode::PLAYING;
        return;
    }

    mode = Mode::RECORDING;
}

void Sequencer::advanceStep() {
    currentStep++;
    if (currentStep >= NUM_STEPS) {
        if (looping) {
            currentStep = 0;
        } else {
            currentStep = NUM_STEPS - 1;
            mode = Mode::STOPPED;
        }
    }
}

void Sequencer::resetPlayhead() {
    currentStep = 0;
}

void Sequencer::movePlayhead(int delta) {
    currentStep += delta;
    if (currentStep < 0) {
        currentStep = 0;
    }
    if (currentStep >= NUM_STEPS) {
        currentStep = NUM_STEPS - 1;
    }
}

void Sequencer::setArmedTrack(int track) {
    if (track < 0 || track >= NUM_TRACKS) return;
    armedTrack = track;
}

void Sequencer::recordMidiEvent(const MidiInputEvent& event) {
    liveInputNote = event.note;
    liveInputActive = event.noteOn;

    if (!event.noteOn || mode != Mode::RECORDING) {
        return;
    }

    const uint8_t velocity = event.velocity == 0 ? inputVelocity : event.velocity;
    setStepNote(armedTrack, currentStep, event.note, velocity);
}

const char* Sequencer::noteToName(uint8_t note) {
    static const char* names[] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };
    static char buf[8];
    snprintf(buf, sizeof(buf), "%s%d", names[note % 12], (note / 12) - 1);
    return buf;
}
