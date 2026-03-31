#include "AudioHandler.hpp"
#include "Instrument.hpp"
#include "Piano.hpp"
#include "Guitar.hpp"
#include "Bass.hpp"
#include "Drums.hpp"
#include "Note.hpp"

int main() {
    AudioHandler audioHandler;

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

    while (true) {
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
    }

    return 0;
}