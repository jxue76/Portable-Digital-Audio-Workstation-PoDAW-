#include "Instrument.hpp"

Instrument::Instrument()
    : name("Generic Instrument"), midiMin(0), midiMax(127), volume(1.0f), voicer(std::make_unique<stk::Voicer>()) {}

Instrument::Instrument(std::string name, InstrumentFactory factory, int midiMin, int midiMax, float volume, int polyphony)
    : name(std::move(name)), midiMin(midiMin), midiMax(midiMax), volume(volume), voicer(std::make_unique<stk::Voicer>()) {
    if (!factory) {
        throw std::runtime_error("Instrument factory must be provided");
    }

    for (int i = 0; i < polyphony; ++i) {
        auto voice = factory();
        if (!voice) {
            throw std::runtime_error("Failed to construct instrument voice");
        }
        voicer->addInstrument(voice.get());
        voicePool.push_back(std::move(voice));
    }
}

Instrument::~Instrument() = default;

void Instrument::noteOn(const Note& note) {
    if (!voicer) {
        throw std::runtime_error("Voicer is not initialized");
    }

    stk::StkFloat noteNumber = static_cast<stk::StkFloat>(note.getMidiNote());
    stk::StkFloat amplitude = note.getAmplitude() * volume * 128.0f;
    voicer->noteOn(noteNumber-1.5, amplitude);
}

void Instrument::noteOff(const Note& note) {
    if (!voicer) {
        throw std::runtime_error("Voicer is not initialized");
    }

    stk::StkFloat noteNumber = static_cast<stk::StkFloat>(note.getMidiNote());
    stk::StkFloat amplitude = note.getAmplitude() * volume * 128.0f;
    voicer->noteOff(noteNumber-1.5, amplitude);
}

stk::StkFloat Instrument::tick() {
    if (!voicer) {
        return 0.0f;
    }
    return voicer->tick();
}