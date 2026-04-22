#pragma once
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <memory>
#include "MidiRecording.hpp"
#include "Instrument.hpp"

class MidiRecorder {
public:

    std::chrono::microseconds getElapsedTime() const {
        std::chrono::microseconds elapsed(0);
        if (!recording) {
            return elapsed;
        }
        auto now = Clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);
        return elapsed;
    }
    void start(); // Clears existing events and starts recording
    MidiRecording stop(); // returns finished recording

    void process(const MidiMessage& msg); // Add midi message to recording with timestamp

    void setInstrument(std::shared_ptr<Instrument> instr) { currentInstrument = instr; }

    bool isRecording() const { return recording; }

private:
    std::vector<TimedMidiMessage> events;
    std::chrono::time_point<Clock> startTime;
    std::shared_ptr<Instrument> currentInstrument;

    std::atomic<bool> recording{false};
    std::mutex mtx;
};