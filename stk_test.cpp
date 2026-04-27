#include <thread>
#include <chrono>
#include <iostream>
#include "AudioHandler.hpp"
#include "Instrument.hpp"
#include "Piano.hpp"
#include "Guitar.hpp"
#include "Bass.hpp"
#include "Drums.hpp"
#include "Note.hpp"


void test0(AudioHandler& audioHandler) {
    
    auto piano = std::make_shared<Piano>();
    auto guitar = std::make_shared<Guitar>();
    auto bass = std::make_shared<Bass>();
    auto drums = std::make_shared<Drums>();

    audioHandler.addInstrument(piano);
    audioHandler.addInstrument(guitar);
    audioHandler.addInstrument(bass);
    audioHandler.addInstrument(drums);

    Note c4(60, 0.8f); // Middle C
    Note e4(64, 0.8f); // E4
    Note g4(67, 0.8f); // G4

    Note kick(36, 1.0f); // Kick drum
    Note snare(38, 1.0f); // Snare drum
    Note hihat(42, 1.0f); // Hi-hat

    int iterations = 0;
    while (iterations < 3) { 
        std::cout << "Iteration " << iterations + 1 << std::endl;
        audioHandler.addNoteToInstrument(piano, c4);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        audioHandler.addNoteToInstrument(guitar, e4);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        audioHandler.addNoteToInstrument(bass, g4);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        audioHandler.removeNoteFromInstrument(piano, c4);
        audioHandler.removeNoteFromInstrument(guitar, e4);
        audioHandler.removeNoteFromInstrument(bass, g4);
        audioHandler.addNoteToInstrument(drums, kick);
        audioHandler.addNoteToInstrument(drums, snare);
        audioHandler.addNoteToInstrument(drums, hihat);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        audioHandler.removeNoteFromInstrument(drums, kick);
        audioHandler.removeNoteFromInstrument(drums, snare);
        audioHandler.removeNoteFromInstrument(drums, hihat);
        audioHandler.printActiveNotes();
        iterations++;
    }
}

void test1(AudioHandler& audioHandler) {

    auto piano = std::make_shared<Piano>();
    auto drums = std::make_shared<Drums>();
    audioHandler.addInstrument(piano);
    audioHandler.addInstrument(drums);
    
    Note c4(60, 0.8f); // Middle C
    Note e4(64, 0.8f); // E4
    Note g4(67, 0.8f); // G4
    Note f4(65, 0.8f); // F4
    Note a4(69, 0.8f); // A4
    Note b4(71, 0.8f); // B4

    Note kick(36, 1.0f); // Kick drum
    Note snare(38, 1.0f); // Snare drum
    Note hihat(42, 1.0f); // Hi-hat


    for (int i = 0; i < 5; i++) {
        audioHandler.addNoteToInstrument(piano, c4);
        audioHandler.addNoteToInstrument(piano, e4);
        audioHandler.addNoteToInstrument(piano, g4);
        audioHandler.addNoteToInstrument(drums, kick);
        audioHandler.addNoteToInstrument(drums, hihat);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        audioHandler.removeNoteFromInstrument(piano, e4);
        audioHandler.removeNoteFromInstrument(piano, g4);
        audioHandler.removeNoteFromInstrument(drums, kick);
        audioHandler.removeNoteFromInstrument(drums, hihat);
        audioHandler.addNoteToInstrument(piano, f4);
        audioHandler.addNoteToInstrument(piano, a4);
        audioHandler.addNoteToInstrument(drums, snare);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        audioHandler.removeNoteFromInstrument(piano, f4);
        audioHandler.removeNoteFromInstrument(piano, a4);
        audioHandler.removeNoteFromInstrument(drums, snare);
        audioHandler.addNoteToInstrument(piano, e4);
        audioHandler.addNoteToInstrument(piano, g4);
        audioHandler.addNoteToInstrument(drums, kick);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        audioHandler.removeNoteFromInstrument(piano, c4);
        audioHandler.removeNoteFromInstrument(piano, e4);
        audioHandler.removeNoteFromInstrument(drums, kick);
        audioHandler.addNoteToInstrument(piano, b4);
        audioHandler.addNoteToInstrument(piano, f4);
        audioHandler.addNoteToInstrument(drums, snare);
        audioHandler.printActiveNotes();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        audioHandler.removeNoteFromInstrument(piano, b4);
        audioHandler.removeNoteFromInstrument(piano, f4);
        audioHandler.removeNoteFromInstrument(drums, snare);
    }
}

int main(int argc, char* argv[]) {
    stk::Stk::setRawwavePath("../stk/rawwaves/");

    AudioHandler audioHandler;
    if (argc > 1 && std::string(argv[1]) == "test0") {
        std::cout << "Running test0..." << std::endl;
        
        test0(audioHandler);
    } else if (argc > 1 && std::string(argv[1]) == "test1") {
        std::cout << "Running test1..." << std::endl;
        test1(audioHandler);
    } else {
        std::cout << "Please specify a test to run: 'test0' or 'test1'" << std::endl;
    }
    return 0;
}