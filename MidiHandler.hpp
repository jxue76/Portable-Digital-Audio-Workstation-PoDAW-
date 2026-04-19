#pragma once
#include "RtMidi.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <vector>
#include "MidiMessage.hpp"

class MidiHandler {
public:
    explicit MidiHandler(bool enableDevice = true);
    virtual ~MidiHandler();

    MidiMessage popMessage(); // Pops the next MIDI message from the queue, returns an empty message if none are available
    bool hasMessages() const; // Checks if there are MIDI messages in the queue
protected:
    void pushMessage(const MidiMessage& msg);
    std::vector<MidiMessage> midiMessages;
private:
    std::unique_ptr<RtMidiIn> midiIn;
};

class TestMidiHandler : public MidiHandler {
public:
    struct ScheduledMidiMessage {
        std::chrono::milliseconds timeOffset;
        MidiMessage message;
    };

    TestMidiHandler(const std::vector<ScheduledMidiMessage>& schedule = {});

    void start();
    void update();
    void reset();

private:
    std::vector<ScheduledMidiMessage> scheduledMessages;
    std::chrono::steady_clock::time_point startTime;
    size_t nextIndex;
};