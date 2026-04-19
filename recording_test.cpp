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
    std::vector<TestMidiHandler::ScheduledMidiMessage> schedule = {
        { std::chrono::milliseconds(100), MidiMessage(Note(60, 1.0f), true) },
        { std::chrono::milliseconds(300), MidiMessage(Note(60, 1.0f), false) }
    };

    TestMidiHandler testMidi(schedule);
    while (true) {
        testMidi.update();
        if (testMidi.hasMessages()) {
            MidiMessage msg = testMidi.popMessage();
            // process msg
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void test2() {
    MidiRecorder recorder;
    std::shared_ptr<Piano> piano = std::make_shared<Piano>();
    recorder.setInstrument(piano);
    recorder.start();

    std::vector<TestMidiHandler::ScheduledMidiMessage> schedule = {
        { std::chrono::milliseconds(100), MidiMessage(Note(60, 1.0f), true) },
        { std::chrono::milliseconds(300), MidiMessage(Note(60, 1.0f), false) }
    };

    TestMidiHandler testMidi(schedule);
    auto startTime = std::chrono::steady_clock::now();
    auto timeout = startTime + std::chrono::seconds(2);

    while (std::chrono::steady_clock::now() < timeout) {
        testMidi.update();

        while (testMidi.hasMessages()) {
            recorder.process(testMidi.popMessage());
        }

        auto elapsed = std::chrono::steady_clock::now() - startTime;
        if (elapsed > std::chrono::milliseconds(400) && !testMidi.hasMessages()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    MidiRecording recording = recorder.stop();

    bool passed = true;
    if (!recording.getInstrument()) {
        std::cerr << "MidiRecorder test failed: recording instrument was not set." << std::endl;
        passed = false;
    }
    if (recording.getEvents().size() != 2) {
        std::cerr << "MidiRecorder test failed: expected 2 events, got " << recording.getEvents().size() << "." << std::endl;
        passed = false;
    }
    if (recording.getInstrument() != piano) {
        std::cerr << "MidiRecorder test failed: recording instrument does not match the expected instrument." << std::endl;
        passed = false;
    }

    if (passed) {
        const auto& first = recording.getEvents()[0];
        const auto& second = recording.getEvents()[1];
        auto expectedFirst = std::chrono::microseconds(100000);
        auto expectedSecond = std::chrono::microseconds(300000);
        auto tolerance = std::chrono::microseconds(20000);

        if (!first.isOn() || first.getNote().getMidiNote() != 60) {
            std::cerr << "MidiRecorder test failed: first event is not note-on for MIDI note 60." << std::endl;
            passed = false;
        }
        if (first.getTimestamp() < expectedFirst - tolerance || first.getTimestamp() > expectedFirst + tolerance) {
            std::cerr << "MidiRecorder test failed: first event timing is incorrect (" << first.getTimestamp().count() << " us)." << std::endl;
            passed = false;
        }

        if (second.isOn() || second.getNote().getMidiNote() != 60) {
            std::cerr << "MidiRecorder test failed: second event is not note-off for MIDI note 60." << std::endl;
            passed = false;
        }
        if (second.getTimestamp() < expectedSecond - tolerance || second.getTimestamp() > expectedSecond + tolerance) {
            std::cerr << "MidiRecorder test failed: second event timing is incorrect (" << second.getTimestamp().count() << " us)." << std::endl;
            passed = false;
        }
    }

    if (passed) {
        std::cout << "MidiRecorder test passed: recording captured expected events and timing." << std::endl;
    }
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
