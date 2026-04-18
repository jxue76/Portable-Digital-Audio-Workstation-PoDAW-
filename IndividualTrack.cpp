#include "imgui.h"
#include "IndividualTrack.hpp"

static constexpr float HEADER_W  = 80.0f;
static constexpr float GRID_X0   = 90.0f;
static constexpr float GRID_Y0   = 10.0f;
static constexpr float CELL_W    = 22.0f;
static constexpr float CELL_H    = 40.0f;
static constexpr float TRACK_PAD = 55.0f;
static constexpr float BAR_Y     = 280.0f;
static constexpr float BAR_H     = 40.0f;
static constexpr float WIN_W     = 480.0f;

void IndividualTrackUI::render(Sequencer& seq, Inputs& inputs, float dt, bool isPlayback) {
    cursor_increment = 440.0f/(static_cast<float>(seq.timeSigNum)*4.0f);
    handleInputs(seq, inputs, dt);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p       = ImGui::GetCursorScreenPos();
    ImVec2 wp      = ImGui::GetWindowPos();

    // Draw notes here
    // 13 notes an octave, 7 octaves max on a piano
    // Number of notes per note chunk: 26, for two octaves per chunk
    // note_selection_vert will move selection by half octaves(7 notes), max note_selection_vert = 9
    float ppn = 280.0f/26.0f; //pixels per note
    for (int i = 0; i < 26; i++) {
        if (i%2==0) dl->AddRectFilled(ImVec2(-10000, i*ppn), ImVec2(10000, i*ppn+ppn), IM_COL32(90,80,30,255));
    }

    if (fastMovement) dl->AddRectFilled(ImVec2(0,0),ImVec2(40,280),IM_COL32(180,130,70,255));
    if (!fastMovement) dl->AddRectFilled(ImVec2(0,0), ImVec2(40, 280), IM_COL32(70, 130, 180, 255));
    char track_num[16];
    snprintf(track_num, sizeof(track_num), "%d", seq.currentTrack);
    dl->AddText(ImVec2(10, 20), IM_COL32(255,255,255,255), track_num);
    // Next is track instrument
    snprintf(track_num, sizeof(track_num), "PNO");
    dl->AddText(ImVec2(10,30), IM_COL32(255,255,255,255), track_num);
    if (isPlayback) {
        snprintf(track_num, sizeof(track_num), "PLAY");
        dl->AddText(ImVec2(10,40), IM_COL32(255,255,255,255), track_num);
    } else {
        snprintf(track_num, sizeof(track_num), "REC");
        dl->AddText(ImVec2(10,40), IM_COL32(255,255,255,255), track_num);
    }

    // Selection for notes from y = 40 -> 280, x width of 40
    float ppo = 240.0f/7.0f; // 7 octaves, two octaves displayed at a time
    dl->AddRectFilled(ImVec2(0,280-(note_selection_vert*ppo/2)),
                        ImVec2(40, 280-(note_selection_vert*ppo/2)-(ppo*2)),
                        IM_COL32(0,0,0,60));

    dl->AddRectFilled(ImVec2(cursor_pos, 0),
                            ImVec2(cursor_pos+3, 470),
                            IM_COL32(90,80,30,200));

    float segmentLength = 480/max_segments;
    dl->AddRectFilled(ImVec2(0,280), ImVec2(480,320), IM_COL32(30,30,30,255));
    dl->AddRectFilled(ImVec2(current_segment*segmentLength,280),
                        ImVec2(current_segment*segmentLength+segmentLength, 320),
                        IM_COL32(255,255,255,50));
}

void IndividualTrackUI::handleInputs(Sequencer& seq, Inputs& inputs, float dt) {
    if (seq.isStopped()) {
        if (fastMovement) {
            if (leftKey.check(inputs.isLeftPressed(), dt)) {
                current_segment -= 1;
                if (current_segment <= 0) {
                    current_segment = 0;
                }
            }
            if (rightKey.check(inputs.isRightPressed(), dt)) {
                current_segment += 1;
                if (current_segment >= max_segments-1) {
                    current_segment = max_segments-1;
                }
            }
        } else {
            if (leftKey.check(inputs.isLeftPressed(), dt)) {
                cursor_pos -= cursor_increment;
                if (cursor_pos <= 40) {
                    if (current_segment <= 0) {
                        cursor_pos = 40;
                    } else {
                        current_segment -= 1;
                        cursor_pos = 480;
                    }
                }
            }
            if (rightKey.check(inputs.isRightPressed(), dt)) {
                cursor_pos += cursor_increment;
                if (cursor_pos >= 480) {
                    if (current_segment >= max_segments-1) {
                        cursor_pos = 480;
                    } else {
                        current_segment += 1;
                        cursor_pos = 40;
                    }
                }
            }
        }
    }
    if (fastMovement) {
        if (upKey.check(inputs.isUpPressed(), dt)) {
            seq.currentTrack -= 1;
            if (seq.currentTrack <= 1) {
                seq.currentTrack = 1;
            }
        }
        if (downKey.check(inputs.isDownPressed(), dt)) {
            seq.currentTrack += 1;
            if (seq.currentTrack >= seq.NUM_TRACKS) {
                seq.currentTrack = seq.NUM_TRACKS;
            }
        }
    } else {
        if (upKey.check(inputs.isUpPressed(), dt)) {
            note_selection_vert += 1;
            if (note_selection_vert >= 9) note_selection_vert = 9;
        }
        if (downKey.check(inputs.isDownPressed(), dt)) {
            note_selection_vert -= 1;
            if (note_selection_vert <= 0) note_selection_vert = 0;
        }
    }

    if (fastKey.check(inputs.isXPressed(), dt)) {
        fastMovement = !fastMovement;
    }
}