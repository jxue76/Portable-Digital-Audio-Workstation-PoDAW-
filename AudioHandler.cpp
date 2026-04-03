#include "AudioHandler.hpp"

int audio_callback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                  double streamTime, RtAudioStreamStatus status, void *userData) {
    AudioHandler* handler = static_cast<AudioHandler*>(userData);
    float *buffer = (float *) outputBuffer;

    for (unsigned int i = 0; i < nBufferFrames; i++)
    {
        float sample = 0.0f;

        for (auto &instrument_notes : handler->readActiveNotes())
        {
            sample += instrument_notes.first->getStkInstrument()->tick();

        }

        sample *= 0.2f;  // avoid clipping

        buffer[i] = sample;
    }

    return 0;
}

AudioHandler::AudioHandler() {
    outputParams.deviceId = audio.getDefaultOutputDevice();
    outputParams.nChannels = 1; // Mono output
    audio.openStream(&outputParams, nullptr, RTAUDIO_FLOAT32, 48000, &bufferFrames, &audio_callback, this);
    audio.startStream();
}
AudioHandler::~AudioHandler() {
    if (audio.isStreamOpen()) {
        audio.stopStream();
        audio.closeStream();
    }
}
bool AudioHandler::addInstrument(std::shared_ptr<Instrument> instrument) {
    if (activeNotes.find(instrument) != activeNotes.end()) {
        return false; // Instrument already exists
    }
    activeNotes[instrument] = std::set<Note>(); // Initialize with an empty set of notes
    return true;
}
bool AudioHandler::removeInstrument(std::shared_ptr<Instrument> instrument) {
    return activeNotes.erase(instrument) > 0; // Returns true if an instrument was removed
}
bool AudioHandler::addNoteToInstrument(std::shared_ptr<Instrument> instrument, Note note) {
    bool inserted = activeNotes[instrument].insert(note).second;
    if (inserted) {
        instrument->getStkInstrument()->noteOn(instrument->midiToFrequency(note.getMidiNote()), note.getAmplitude() * instrument->getVolume());
    }
    return inserted;
}

bool AudioHandler::removeNoteFromInstrument(std::shared_ptr<Instrument> instrument, Note note) {
    auto it = activeNotes.find(instrument);
    if (it != activeNotes.end()) {
        size_t erased = it->second.erase(note);
        if (erased > 0) {
            instrument->getStkInstrument()->noteOff(instrument->midiToFrequency(note.getMidiNote()));
            return true;
        }
    }
    return false;
}

void AudioHandler::printActiveNotes() const {
    for (const auto &instrument_notes : activeNotes) {
        std::cout << "Instrument: " << instrument_notes.first->getName() << " - Active Notes: ";
        for (const auto &note : instrument_notes.second) {
            std::cout << note.getMidiNote() << " (" << note.getAmplitude() << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------" << std::endl;
}