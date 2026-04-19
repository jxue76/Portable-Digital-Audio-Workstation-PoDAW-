#pragma once
#include <vector>
#include <chrono>
#include <memory>
#include "MidiMessage.hpp"
#include "Instrument.hpp"

using Clock = std::chrono::steady_clock;

class TimedMidiMessage : public MidiMessage {
public:
    TimedMidiMessage(const MidiMessage& m, std::chrono::microseconds ts)
        : MidiMessage(m), timestamp(ts) {}
    
    std::chrono::microseconds getTimestamp() const { return timestamp; }

private:
    std::chrono::microseconds timestamp;
};

class MidiRecording {
public:
    MidiRecording() = default;
    MidiRecording(const MidiRecording& other) = default;
    std::vector<TimedMidiMessage>& getEvents() { return events; }
    const std::vector<TimedMidiMessage>& getEvents() const { return events; }
    std::chrono::microseconds getLength() const { return length; }
    std::shared_ptr<Instrument> getInstrument() const { return instrument; }

    void setInstrument(std::shared_ptr<Instrument> instr) { instrument = instr; }
    void setEvents(const std::vector<TimedMidiMessage>& ev) { events = ev; }
    void setLength(std::chrono::microseconds len) { length = len; }

private:
    std::vector<TimedMidiMessage> events;
    std::chrono::microseconds length{0};
    std::shared_ptr<Instrument> instrument;
};