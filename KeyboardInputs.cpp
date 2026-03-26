/*
This file implements the KeyboardInputs class, which implements the Inputs interface to read button states from a keyboard device. It uses the Linux input event system to read key events and updates the state of the buttons accordingly. The class also handles opening the appropriate input device and cleaning up resources when done.
THIS HAS NOT YET BEEN SUCCESSFULLY TESTED, maybe we re-implement using imgui if necessary
*/
#include "KeyboardInputs.hpp"
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>

KeyboardInputs::KeyboardInputs() : Inputs(){
    // Try to open keyboard device
    const char* devices[] = {"/dev/input/event0", "/dev/input/event1", "/dev/input/event2", "/dev/input/event3", "/dev/input/event4", "/dev/input/event5"};
    fd = -1;
    for (const char* device : devices) {
        fd = open(device, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            std::cerr << "Opened keyboard device: " << device << std::endl;
            break;
        }
    }
    if (fd < 0) {
        std::cerr << "Failed to open any keyboard device" << std::endl;
        throw std::runtime_error("Failed to open keyboard device");
    }

    // Initialize key states
    keyStates[103] = false; // KEY_UP
    keyStates[108] = false; // KEY_DOWN
    keyStates[105] = false; // KEY_LEFT
    keyStates[106] = false; // KEY_RIGHT
    keyStates[30] = false;  // KEY_A
    keyStates[31] = false;  // KEY_S (B)
    keyStates[32] = false;  // KEY_D (X)
    keyStates[24] = false;  // KEY_O
    keyStates[25] = false;  // KEY_P
}

KeyboardInputs::~KeyboardInputs() {
    // Close device
    close(fd);
}

void KeyboardInputs::updateKeyStates() const {
    struct input_event ev;
    ssize_t n;
    while ((n = read(fd, &ev, sizeof(ev))) > 0) {
        if (ev.type == EV_KEY) {
            keyStates[ev.code] = (ev.value != 0);
        }
    }
}

bool KeyboardInputs::isUpPressed() const {
    updateKeyStates();
    return keyStates[103];
}

bool KeyboardInputs::isDownPressed() const {
    updateKeyStates();
    return keyStates[108];
}

bool KeyboardInputs::isLeftPressed() const {
    updateKeyStates();
    return keyStates[105];
}

bool KeyboardInputs::isRightPressed() const {
    updateKeyStates();
    return keyStates[106];
}

bool KeyboardInputs::isAPressed() const {
    updateKeyStates();
    return keyStates[30];
}

bool KeyboardInputs::isBPressed() const {
    updateKeyStates();
    return keyStates[31];
}

bool KeyboardInputs::isXPressed() const {
    updateKeyStates();
    return keyStates[32];
}

Dial KeyboardInputs::getDialPosition() const {
    updateKeyStates();
    if (keyStates[24]) {
        return Dial::UP;
    } else if (keyStates[25]) {
        return Dial::DOWN;
    } else {
        return Dial::NEUTRAL;
    }
}