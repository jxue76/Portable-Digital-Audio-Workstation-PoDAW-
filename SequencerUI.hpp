#ifndef SEQUENCER_UI_HPP
#define SEQUENCER_UI_HPP

#include "Sequencer.hpp"
#include "Inputs.hpp"

class SequencerUI {
public:
    void render(Sequencer& seq, Inputs& inputs, float dt);

private:
    struct KeyRepeat {
        bool  wasDown = false;
        float timer   = 0.0f;

        bool check(bool isDown, float deltaTime) {
            if (!isDown) {
                wasDown = false;
                timer   = 0.0f;
                return false;
            }
            if (!wasDown) {
                wasDown = true;
                timer   = 0.0f;
                return true;
            }
            timer += deltaTime;
            if (timer >= 0.35f) {
                timer -= 0.07f;
                return true;
            }
            return false;
        }
    };

    KeyRepeat upKey, downKey, leftKey, rightKey;
    bool prevA = false;
    bool prevB = false;
    bool prevX = false;

    float stepTimer = 0.0f;

    void handleInput(Sequencer& seq, Inputs& inputs, float dt);
    void updatePlayback(Sequencer& seq, float dt);
};

#endif
