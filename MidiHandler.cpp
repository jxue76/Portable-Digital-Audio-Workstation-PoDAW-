#include "MidiHandler.hpp"

void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    MidiHandler* handler = static_cast<MidiHandler*>(userData);

    std::cout << "Received MIDI message" <<std::endl;
    
    if (message->size() >= 3) {
        unsigned char status = message->at(0);
        unsigned char data1 = message->at(1);
        unsigned char data2 = message->at(2);
        
        // Check if it's a note on or note off message
        bool isNoteOn = (status >= 0x90 && status <= 0x9F);
        bool isNoteOff = (status >= 0x80 && status <= 0x8F) || (isNoteOn && data2 == 0);

        /*std::cout << "Received MIDI message: Status " << std::hex << (int)status << std::dec 
                  << ", Data1 " << (int)data1 << ", Data2 " << (int)data2 
                  << " (Note " << (int)data1 << ", " << (isNoteOn ? "ON" : "OFF") << ")" << std::endl;
        */
        if (isNoteOn || isNoteOff) {
            // Extract channel (lower 4 bits of status)
            unsigned char channel = status & 0x0F;
            
            // Create Note object
            Note note(data1, 1.0f);
            
            // Create MidiMessage
            MidiMessage midiMsg(note, isNoteOn && data2 > 0);
            
            // Add to queue (thread-safe if needed)
            handler->pushMessage(midiMsg);
        }
        // Ignore all other message types
    }
}

MidiHandler::MidiHandler(bool enableDevice) {
    if (enableDevice) {
        std::cout << midiIn.getPortCount() << " MIDI input ports available." << std::endl;
        for (unsigned int i = 0; i < midiIn.getPortCount(); i++) {
            std::string portName = midiIn.getPortName(i);
            std::cout << "Port " << i << ": " << portName << std::endl;
        }
        if (midiIn.getPortCount() > 0) {
            midiIn.openPort(1); // Open the first available MIDI port
            midiIn.ignoreTypes(true, true, true); // Ignore sysex, timing, and active sensing messages
            midiIn.setCallback(&midiCallback, this);
            std::cout << "MIDI input device initialized and callback set." << std::endl;
        } else {
            std::cerr << "No MIDI input ports available!" << std::endl;
        }
    }
}

MidiHandler::~MidiHandler() {
    if (midiIn.isPortOpen()) {
        midiIn.closePort();
    }
}

void MidiHandler::pushMessage(const MidiMessage& msg) {
    midiMessages.push_back(msg);
    printf("Pushed MIDI message: Note %d, %s\n", msg.getNote().getMidiNote(), msg.isOn() ? "ON" : "OFF");
}

MidiMessage MidiHandler::popMessage() {
    if (!midiMessages.empty()) {
        MidiMessage msg = midiMessages.front();
        midiMessages.erase(midiMessages.begin());
        return msg;
    }
    return MidiMessage(); // Return empty message if none available
}

bool MidiHandler::hasMessages() const {
    return !midiMessages.empty();
}

TestMidiHandler::TestMidiHandler(const std::vector<ScheduledMidiMessage>& schedule)
    : MidiHandler(false), scheduledMessages(schedule), nextIndex(0) {
    std::sort(scheduledMessages.begin(), scheduledMessages.end(), [](const ScheduledMidiMessage& a, const ScheduledMidiMessage& b) {
        return a.timeOffset < b.timeOffset;
    });
    start();
}

void TestMidiHandler::start() {
    startTime = std::chrono::steady_clock::now();
    nextIndex = 0;
}

void TestMidiHandler::update() {
    if (nextIndex >= scheduledMessages.size()) {
        return;
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
    while (nextIndex < scheduledMessages.size() && scheduledMessages[nextIndex].timeOffset <= elapsed) {
        pushMessage(scheduledMessages[nextIndex].message);
        nextIndex++;
    }
}

void TestMidiHandler::reset() {
    start();
}
