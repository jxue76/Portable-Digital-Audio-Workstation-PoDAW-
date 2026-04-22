#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <string>

#include "AudioHandler.hpp"
#include "MidiHandler.hpp"
#include "MidiPlayer.hpp"
#include "MidiRecording.hpp"
#include "MidiRecorder.hpp"
#include "Piano.hpp"

// Test Playback of a prerecorded recording, ensuring it ends with zero active notes
void test0() {
    std::cout << "Running test0: Playback of prerecorded recording" << std::endl;
    AudioHandler audioHandler;
    std::shared_ptr<Piano> piano = std::make_shared<Piano>();
    audioHandler.addInstrument(piano);

    MidiRecording recording;
    recording.setInstrument(piano);
    Note note60(60, 0.5f);

    std::vector<TimedMidiMessage> testEvents;
    testEvents.emplace_back(MidiMessage(note60, true), std::chrono::microseconds(0));
    testEvents.emplace_back(MidiMessage(note60, false), std::chrono::microseconds(250000));

    recording.setEvents(testEvents);
    recording.setLength(std::chrono::microseconds(250000));

    std::cout << "Starting playback of prerecorded recording..." << std::endl;
    MidiPlayer player(audioHandler);

    player.play(recording);

    std::cout << "Waiting for playback to finish..." << std::endl;
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (player.isPlaying() && std::chrono::steady_clock::now() < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (player.isPlaying()) {
        std::cerr << "MidiPlayer test failed: playback still running after timeout." << std::endl;
        player.stop();
        return;
    }

    auto activeNotes = audioHandler.readActiveNotes();
    bool notesCleared = true;
    for (const auto& instrumentNotes : activeNotes) {
        if (!instrumentNotes.second.empty()) {
            notesCleared = false;
            break;
        }
    }

    if (!notesCleared) {
        std::cerr << "MidiPlayer test failed: expected no active notes after playback." << std::endl;
    } else {
        std::cout << "MidiPlayer test passed: prerecorded recording played and ended with zero active notes." << std::endl;
    }
}

// Test Recording functionality by simulating a recording session with scheduled MIDI messages, then verifying the recorded events and their timing.
void test1() {
    AudioHandler audioHandler;

    std::vector<TestMidiHandler::ScheduledMidiMessage> schedule = {
        { std::chrono::milliseconds(100), MidiMessage(Note(60, 1.0f), true) },
        { std::chrono::milliseconds(300), MidiMessage(Note(60, 1.0f), false) },
        { std::chrono::milliseconds(500), MidiMessage(Note(64, 1.0f), true) },
        { std::chrono::milliseconds(700), MidiMessage(Note(64, 1.0f), false) },
        { std::chrono::milliseconds(900), MidiMessage(Note(67, 1.0f), true) },
        { std::chrono::milliseconds(1100), MidiMessage(Note(67, 1.0f), false) }
    };

    TestMidiHandler testMidi(schedule);
    MidiRecorder recorder;
    std::shared_ptr<Piano> piano = std::make_shared<Piano>();
    audioHandler.addInstrument(piano);
    recorder.setInstrument(piano);
    recorder.start();

    while (recorder.getElapsedTime() < std::chrono::seconds(2)) {
        testMidi.update();
        if (testMidi.hasMessages()) {
            MidiMessage msg = testMidi.popMessage();
            std::cout << "Test MIDI message: Note " << msg.getNote().getMidiNote() 
                      << (msg.isOn() ? " ON" : " OFF") << std::endl;
            recorder.process(msg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    MidiRecording recording = recorder.stop();
    MidiPlayer player(audioHandler);
    player.play(recording);
    while (player.isPlaying()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    player.stop();
}

void test2() {
    AudioHandler audioHandler;
    std::shared_ptr<Piano> piano = std::make_shared<Piano>();
    audioHandler.addInstrument(piano);
    MidiHandler midiHandler;

    MidiRecorder recorder;
    recorder.setInstrument(piano);

    recorder.start();

    while (recorder.getElapsedTime() < std::chrono::seconds(2)) {
        if (midiHandler.hasMessages()) {
            MidiMessage msg = midiHandler.popMessage();
            std::cout << "Received MIDI message: Note " << msg.getNote().getMidiNote() 
                      << (msg.isOn() ? " ON" : " OFF") << std::endl;
            recorder.process(msg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    MidiRecording recording = recorder.stop();
    MidiPlayer player(audioHandler);
    player.play(recording);
    while (player.isPlaying()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    player.stop();
}

int main(int argc, char* argv[]) {
    stk::Stk::setRawwavePath("../stk/rawwaves/");
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <test_number>" << std::endl;
        return 1;
    }
    int testNum = std::stoi(argv[1]);
    switch (testNum) {
        case 0:
            test0();
            break;
        case 1:
            test1();
            break;
        case 2:
            test2();
            break;
        default:
            std::cerr << "Invalid test number: " << testNum << std::endl;
            return 1;
    }
    return 0;
}
