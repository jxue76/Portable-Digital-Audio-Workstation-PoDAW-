#ifndef IDIVIDUALTRACKUI_HPP
#define INDIVIDUALTRACKUI_HPP

#include "Inputs.hpp"
#include "Sequencer.hpp"
#include "MidiRecording.hpp"

class IndividualTrackUI {
    public:
        void render(Sequencer& seq, Inputs& inputs, float dt,
                    std::vector<MidiRecording>& recordedNotes,
                    double& cursorPosition);
        float returnPPB() {return ppb;}
        void drawNotes(MidiRecording& recordedNotes, Sequencer& seq);
        void pushCursorPlayback(Sequencer& seq, double dt, MidiRecording& recordedNotes);
        void setCursor(float newCursor) {cursor_pos = newCursor;}
        
        bool playback = true;
        bool isMoving = false;

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
        
        float cursor_pos = 60.0f;
        
        int current_segment = 0;
        int max_segments = 4;


        int note_selection_vert = 5;
        float ppn = 420.0f/26.0f; // Pixels per note vertial
        float ppb = 0.0f; // Pixels per beat horizontal

        float cursor_increment;
        ImU32 noteCol = IM_COL32(200,200,200,255);;

        float x_end;
        float y;

        std::vector<std::vector<ImVec2>> drawnNotes;

        KeyRepeat upKey, downKey, leftKey, rightKey, fastKey;
        bool fastMovement = false;
        void handleInputs(Sequencer& seq, Inputs& inputs, float dt, std::vector<MidiRecording>& recordedNotes);
};

#endif