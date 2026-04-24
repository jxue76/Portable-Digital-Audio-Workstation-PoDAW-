#include "MidiPlayer.hpp"
#include <algorithm>

MidiPlayer::MidiPlayer(AudioHandler& audioHandler)
    : audio(audioHandler) {}

void MidiPlayer::play(const MidiRecording& recording, bool loop, double offset) {
    if (recording.getEvents().empty()) return;

    stop(); // stop existing playback if any

    //std::cout << "Starting playback of recording with " << recording.getEvents().size() << " events." << std::endl;
    // Create a copy and sort by timestamp
    MidiRecording sortedRecording(recording);

    //std::cout << "Sorting recording events by timestamp..." << std::endl;
    std::sort(sortedRecording.getEvents().begin(), sortedRecording.getEvents().end(), 
              [](const TimedMidiMessage& a, const TimedMidiMessage& b) {
                  return a.getTimestamp() < b.getTimestamp();
              });

    //std::cout << "Recording sorted" << std::endl;

    playing = true;

    std::cout << "Launching playback thread..." << std::endl;
    playbackThread = std::thread([this, sortedRecording, loop, offset]() {
        playbackThreadFunc(sortedRecording, loop, offset);
    });
}

void MidiPlayer::stop() {
    //if (!playing) return;
    playing = false;
    if (playbackThread.joinable())
        playbackThread.join();
}

void MidiPlayer::playbackThreadFunc(MidiRecording recording, bool loop = false, double offset = 0.0) {
    std::chrono::microseconds startOffset = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(offset));
    //std::cout << "Start offset microseconds: " << startOffset.count() << std::endl;
    std::cout << "Length: " << recording.getEvents().size() << std::endl;
    
    do {
        auto start = Clock::now();

        for (const auto& ev : recording.getEvents()) {
            if (!playing) return;

            auto target = start + ev.getTimestamp() - startOffset;
            if (ev.getTimestamp().count() >= startOffset.count()) {
                std::this_thread::sleep_until(target);

                if (ev.isOn()) {
                    audio.addNoteToInstrument(recording.getInstrument(), ev.getNote());
                } else {
                    audio.removeNoteFromInstrument(recording.getInstrument(), ev.getNote());
                }
            }
        }

        playing = false;

    } while (loop || playing);

    playing = false;
}
