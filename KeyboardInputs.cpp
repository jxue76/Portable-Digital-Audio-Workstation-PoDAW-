#include "KeyboardInputs.hpp"
#include <GLFW/glfw3.h>

KeyboardInputs::KeyboardInputs(GLFWwindow* w) : Inputs(), window(w) {}

namespace {
struct NoteKey {
    int glfwKey;
    int semitone;
};

constexpr NoteKey kNoteKeys[] = {
    {GLFW_KEY_A,  0},
    {GLFW_KEY_W,  1},
    {GLFW_KEY_S,  2},
    {GLFW_KEY_E,  3},
    {GLFW_KEY_D,  4},
    {GLFW_KEY_F,  5},
    {GLFW_KEY_T,  6},
    {GLFW_KEY_G,  7},
    {GLFW_KEY_Y,  8},
    {GLFW_KEY_H,  9},
    {GLFW_KEY_U, 10},
    {GLFW_KEY_J, 11},
    {GLFW_KEY_K, 12},
};
}

bool KeyboardInputs::isUpPressed() const {
    return glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
}

bool KeyboardInputs::isDownPressed() const {
    return glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
}

bool KeyboardInputs::isLeftPressed() const {
    return glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
}

bool KeyboardInputs::isRightPressed() const {
    return glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
}

bool KeyboardInputs::isAPressed() const {
    return glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
}

bool KeyboardInputs::isBPressed() const {
    return glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS;
}

bool KeyboardInputs::isXPressed() const {
    return glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
}

Dial KeyboardInputs::getDialPosition() const {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) return Dial::UP;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) return Dial::DOWN;
    return Dial::NEUTRAL;
}

bool KeyboardInputs::pollMidiInputEvent(MidiInputEvent& event) const {
    syncMidiState();
    if (pendingMidiEvents.empty()) {
        return false;
    }

    event = pendingMidiEvents.front();
    pendingMidiEvents.pop_front();
    return true;
}

void KeyboardInputs::syncMidiState() const {
    const bool octaveDownPressed = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
    const bool octaveUpPressed = glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS;

    if (octaveDownPressed && !octaveDownWasPressed && keyboardOctave > 0) {
        keyboardOctave--;
    }
    if (octaveUpPressed && !octaveUpWasPressed && keyboardOctave < 8) {
        keyboardOctave++;
    }

    octaveDownWasPressed = octaveDownPressed;
    octaveUpWasPressed = octaveUpPressed;

    for (size_t i = 0; i < 13; ++i) {
        const bool pressed = glfwGetKey(window, kNoteKeys[i].glfwKey) == GLFW_PRESS;

        if (pressed && !noteWasPressed[i]) {
            const uint8_t note = static_cast<uint8_t>(((keyboardOctave + 1) * 12) + kNoteKeys[i].semitone);
            activeNotes[i] = note;
            pendingMidiEvents.push_back(MidiInputEvent{note, 100, true});
        } else if (!pressed && noteWasPressed[i]) {
            pendingMidiEvents.push_back(MidiInputEvent{activeNotes[i], 0, false});
        }

        noteWasPressed[i] = pressed;
    }
}
