#include "MidiRecording.hpp"
#include "MidiRecorder.hpp"

void MidiRecorder::start() {
    std::lock_guard<std::mutex> lock(mtx);
    events.clear();
    startTime = Clock::now();
    recording = true;
}

MidiRecording MidiRecorder::stop() {
    std::lock_guard<std::mutex> lock(mtx);
    recording = false;

    MidiRecording rec;
    rec.setEvents(events);

    if (!events.empty()) {
        rec.setLength(Clock::now() - startTime);
    }
    
    rec.setInstrument(currentInstrument);

    return rec;
}

void MidiRecorder::process(const MidiMessage& msg) {
    if (!recording || !currentInstrument) return;

    auto now = Clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);

    std::lock_guard<std::mutex> lock(mtx);
    events.push_back(TimedMidiMessage(msg, delta));
}