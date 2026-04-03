#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <stk/Instrmnt.h>


class Instrument {
public:
    Instrument();
    Instrument(std::string name, std::shared_ptr<stk::Instrmnt> stkInstrument, int midiMin, int midiMax, float volume);
    virtual ~Instrument();
    virtual stk::StkFloat midiToFrequency(int midiNote) const {
        midiNote = mapBetween(midiNote, midiMin, midiMax);
        return 440.0 * pow(2.0, (midiNote - 69) / 12.0); // Standard MIDI to frequency conversion
    }

    inline std::string getName() const {
        return name;
    }
    std::shared_ptr<stk::Instrmnt> getStkInstrument() const;
    inline float getVolume() const {
        return volume;
    }
    inline void setVolume(float newVolume) {
        volume = newVolume;
    }

    static int mapBetween(int x, int a, int b) {
        int range = b - a + 1;
        int offset = x - a;
        int modded = (offset % range + range) % range; // % is not a true modulo operator in C++, so we adjust for negative values
        return a + modded;
    }
private:
    std::string name;
    std::shared_ptr<stk::Instrmnt> stkInstrument;
    float volume;
    int midiMin;
    int midiMax;
};