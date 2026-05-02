#pragma once

#include <string>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <vector>
#include "Instrmnt.h"
#include "Voicer.h"
#include "Note.hpp"

class Instrument {
public:
    using InstrumentFactory = std::function<std::unique_ptr<stk::Instrmnt>()>;

    Instrument();
    Instrument(std::string name, InstrumentFactory factory, int midiMin, int midiMax, float volume, int polyphony = 8);
    virtual ~Instrument();

    virtual stk::StkFloat midiToFrequency(int midiNote) const {
        midiNote = mapBetween(midiNote, midiMin, midiMax);
        return 440.0 * pow(2.0, (midiNote - 68) / 12.0); // Standard MIDI to frequency conversion
    }

    inline std::string getName() const {
        return name;
    }

    inline float getVolume() const {
        return volume;
    }

    inline void setVolume(float newVolume) {
        volume = newVolume;
    }

    virtual void noteOn(const Note& note);
    virtual void noteOff(const Note& note);
    stk::StkFloat tick();

    static int mapBetween(int x, int a, int b) {
        int range = b - a + 1;
        int offset = x - a;
        int modded = (offset % range + range) % range; // % is not a true modulo operator in C++, so we adjust for negative values
        return a + modded;
    }

protected:
    int midiMin;
    int midiMax;
    stk::StkFloat tuningOffset = 1.5f;
private:
    std::string name;
    float volume;
    std::vector<std::unique_ptr<stk::Instrmnt>> voicePool;
    std::unique_ptr<stk::Voicer> voicer;
};