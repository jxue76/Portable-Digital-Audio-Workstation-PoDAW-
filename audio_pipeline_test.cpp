#include <iostream>
#include <thread>

#include "MidiHandler.hpp"
#include "AudioHandler.hpp"
#include "Piano.hpp"

int main() {
    stk::Stk::setRawwavePath("../stk/rawwaves/");
    MidiHandler midiHandler;
    AudioHandler audioHandler;

    std::shared_ptr<Piano> mainPiano = std::make_shared<Piano>();
    audioHandler.addInstrument(mainPiano);

    while (true) {
        if (midiHandler.hasMessages()) {
            MidiMessage msg = midiHandler.popMessage();
            std::cout << "Received MIDI message: Note " << msg.getNote().getMidiNote() 
                      << (msg.isOn() ? " ON" : " OFF") << std::endl;
            if (msg.isOn()) {
                audioHandler.addNoteToInstrument(mainPiano, msg.getNote());
            } else {
                audioHandler.removeNoteFromInstrument(mainPiano, msg.getNote());
            }
            audioHandler.printActiveNotes(); // For debugging
        }
        // Sleep briefly to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}