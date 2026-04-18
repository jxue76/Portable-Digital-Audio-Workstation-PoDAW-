#ifndef IDIVIDUALTRACKUI_HPP
#define INDIVIDUALTRACKUI_HPP

#include "Inputs.hpp"
#include "Sequencer.hpp"

class IndividualTrackUI {
    public:
        void render(Sequencer& seq, Inputs& inputs, float dt, bool isPlayback);
        //void renderMIDI();
        //void update_info();
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

        float cursor_pos = 40;

        int current_segment = 0;
        int max_segments = 10;
        bool playing = false;

        int note_selection_vert = 4;

        float cursor_increment;

        KeyRepeat upKey, downKey, leftKey, rightKey, fastKey;
        bool fastMovement = false;
        void handleInputs(Sequencer& seq, Inputs& inputs, float dt);
};

#endif