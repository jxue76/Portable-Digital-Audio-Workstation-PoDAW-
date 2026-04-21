#ifndef KEYBOARD_INPUTS_HPP
#define KEYBOARD_INPUTS_HPP

#include "Inputs.hpp"
#include <array>
#include <deque>

struct GLFWwindow;

class KeyboardInputs : public Inputs {
public:
    explicit KeyboardInputs(GLFWwindow* window);
    ~KeyboardInputs() override = default;

    bool isUpPressed()    const override;
    bool isDownPressed()  const override;
    bool isLeftPressed()  const override;
    bool isRightPressed() const override;
    bool isAPressed()     const override;
    bool isBPressed()     const override;
    bool isXPressed()     const override;
    Dial getDialPosition() const override;
    bool pollMidiInputEvent(MidiInputEvent& event) const override;

private:
    GLFWwindow* window;
    mutable std::deque<MidiInputEvent> pendingMidiEvents;
    mutable int keyboardOctave = 4;
    mutable bool octaveDownWasPressed = false;
    mutable bool octaveUpWasPressed = false;
    mutable std::array<bool, 13> noteWasPressed = {};
    mutable std::array<uint8_t, 13> activeNotes = {};

    void syncMidiState() const;
};

#endif
