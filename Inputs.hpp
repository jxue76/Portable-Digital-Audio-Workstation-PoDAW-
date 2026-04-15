#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <cstdint>

enum class Dial {
    UP,
    DOWN,
    NEUTRAL
};

struct MidiInputEvent {
    uint8_t note = 60;
    uint8_t velocity = 100;
    bool noteOn = true;
};

class Inputs {
protected:
    Inputs();
public:
    virtual ~Inputs() = default;
    virtual bool isUpPressed() const;
    virtual bool isDownPressed() const;
    virtual bool isLeftPressed() const;
    virtual bool isRightPressed() const;
    virtual bool isAPressed() const;
    virtual bool isBPressed() const;
    virtual bool isXPressed() const;
    virtual Dial getDialPosition() const;
    virtual bool pollMidiInputEvent(MidiInputEvent& event) const;
};

#endif