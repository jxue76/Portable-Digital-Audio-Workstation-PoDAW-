#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include "MidiRecording.hpp"
#include "AudioHandler.hpp"

class MidiPlayer {
public:
    MidiPlayer(AudioHandler& audioHandler);

    void play(const MidiRecording& recording, bool loop = false, double offset=0.0);
    void stop();

    bool isPlaying() const { return playing; }

private:
    AudioHandler& audio;
    std::thread playbackThread;
    std::atomic<bool> playing{false};

    void playbackThreadFunc(MidiRecording recording, bool loop, double offset);
};