#pragma once
#include <thread>
#include <atomic>
#include "MidiRecording.hpp"
#include "AudioHandler.hpp"

class MidiPlayer {
public:
    MidiPlayer(AudioHandler& audio);

    void play(const MidiRecording& recording, bool loop = false);
    void stop();

    bool isPlaying() const { return playing; }

private:
    AudioHandler& audio;
    std::thread playbackThread;
    std::atomic<bool> playing{false};

    void playbackThreadFunc(MidiRecording recording, bool loop);
};