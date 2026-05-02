#include "imgui.h"
#include "IndividualTrack.hpp"
#include "math.h"

static constexpr float HEADER_W  = 80.0f;
static constexpr float GRID_X0   = 90.0f;
static constexpr float GRID_Y0   = 10.0f;
static constexpr float CELL_W    = 22.0f;
static constexpr float CELL_H    = 40.0f;
static constexpr float TRACK_PAD = 55.0f;
static constexpr float BAR_Y     = 280.0f;
static constexpr float BAR_H     = 40.0f;
static constexpr float WIN_W     = 480.0f;

void IndividualTrackUI::render(Sequencer& seq, Inputs& inputs, float dt, std::vector<MidiRecording>& recordedNotes, double& cursorPosition) {
    cursor_increment = 660.0f/(static_cast<float>(seq.timeSigNum)*4.0f);
    //ppn = 280.0f/26.0f; //pixels per note
    ppb = cursor_increment;
    handleInputs(seq, inputs, dt, recordedNotes);
    cursorPosition = cursor_pos+current_segment*660.0;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p       = ImGui::GetCursorScreenPos();
    ImVec2 wp      = ImGui::GetWindowPos();

    // Draw notes here
    // 13 notes an octave, 7 octaves max on a piano
    // Number of notes per note chunk: 26, for two octaves per chunk
    // note_selection_vert will move selection by half octaves(7 notes), max note_selection_vert = 10
    //drawNotes(recordedNotes, seq);

    for (int i = 0; i < drawnNotes.size(); i++) {
        dl->AddRectFilled(drawnNotes[i][0], drawnNotes[i][1], noteCol);
    }


    if (!isMoving){
        if (fastMovement) dl->AddRectFilled(ImVec2(p.x+0,p.y+0),ImVec2(p.x+60,p.y+420),IM_COL32(180,130,70,255));
        else if (!fastMovement) dl->AddRectFilled(ImVec2(p.x+0,p.y+0), ImVec2(p.x+60, p.x+420), IM_COL32(70, 130, 180, 255));}
    else if (isMoving) {
        dl->AddRectFilled(ImVec2(p.x+0,0),ImVec2(p.x+60,p.y+420),IM_COL32(100,100,100,255));}
    char track_num[32];
    snprintf(track_num, sizeof(track_num), "%d", seq.currentTrack);
    dl->AddText(ImVec2(p.x+10, p.y+20), IM_COL32(255,255,255,255), track_num);
    // Next is track instrument
    if (recordedNotes[seq.currentTrack-1].getInstrument()) {
        snprintf(track_num, sizeof(track_num), "%s", (recordedNotes[seq.currentTrack-1].getInstrument().get()->getName()).c_str());
    }
    else snprintf(track_num, sizeof(track_num), "%s", "NUL");
    dl->AddText(ImVec2(p.x+10,p.y+40), IM_COL32(255,255,255,255), track_num);
    if (playback) {
        snprintf(track_num, sizeof(track_num), "PLAY");
        dl->AddText(ImVec2(p.x+10,p.y+60), IM_COL32(255,255,255,255), track_num);
    } else {
        snprintf(track_num, sizeof(track_num), "REC");
        dl->AddText(ImVec2(p.x+10,p.y+60), IM_COL32(255,255,255,255), track_num);
    }

    // Selection for notes from y = 40 -> 280, x width of 40
    float ppo = 360.0f/7.0f; // 7 octaves, two octaves displayed at a time
    dl->AddRectFilled(ImVec2(p.x+0,p.y+420-(note_selection_vert*ppo/2)),
                        ImVec2(p.x+60, p.y+420-(note_selection_vert*ppo/2)-(ppo*2)),
                        IM_COL32(0,0,0,50));

    // Cursor
    dl->AddRectFilled(ImVec2(p.x+cursor_pos, p.y+0),
                            ImVec2(p.x+cursor_pos+20, p.y+480),
                            IM_COL32(90,80,30,200));

    float segmentLength = 480/(max_segments-1);
    if (!isMoving) dl->AddRectFilled(ImVec2(p.x+0,p.y+420), ImVec2(p.x+720,p.y+480), IM_COL32(60,60,60,255));
    if (isMoving) {
        if (playback) dl->AddRectFilled(ImVec2(p.x+0,p.y+420), ImVec2(p.x+720,p.y+480), IM_COL32(30,30,180,255));
        if (!playback) dl->AddRectFilled(ImVec2(p.x+0,p.y+420), ImVec2(p.x+720,p.y+480), IM_COL32(180,30,30,255));
    }
    dl->AddRectFilled(ImVec2(p.x+current_segment*segmentLength,p.y+420),
                        ImVec2(p.x+current_segment*segmentLength+segmentLength, p.y+480),
                        IM_COL32(255,255,255,50));
}

void IndividualTrackUI::handleInputs(Sequencer& seq, Inputs& inputs, float dt, std::vector<MidiRecording>& recordedNotes) {
    if (!isMoving) {
        if (fastMovement) {
            if (leftKey.check(inputs.isLeftPressed(), dt)) {
                current_segment -= 1;
                drawNotes(recordedNotes[seq.currentTrack-1], seq);
                if (current_segment <= 0) {
                    current_segment = 0;
                }
            }
            if (rightKey.check(inputs.isRightPressed(), dt)) {
                current_segment += 1;
                drawNotes(recordedNotes[seq.currentTrack-1], seq);
                if (current_segment >= max_segments-1) {
                    current_segment = max_segments-1;
                }
            }
        } else {
            if (leftKey.check(inputs.isLeftPressed(), dt)) {
                cursor_pos -= cursor_increment;
                drawNotes(recordedNotes[seq.currentTrack-1], seq);
                if (cursor_pos < 60) {
                    if (current_segment <= 0) {
                        cursor_pos = 60;
                    } else {
                        current_segment -= 1;
                        cursor_pos = 720 - cursor_increment;
                    }
                }
            }
            if (rightKey.check(inputs.isRightPressed(), dt)) {
                cursor_pos += cursor_increment;
                drawNotes(recordedNotes[seq.currentTrack-1], seq);
                if (cursor_pos > 720 - cursor_increment) {
                    if (current_segment >= max_segments-1) {
                        cursor_pos = 720 - cursor_increment;
                    } else {
                        current_segment += 1;
                        cursor_pos = 60;
                    }
                }
            }
        }
    }
    if (fastMovement) {
        if (upKey.check(inputs.isUpPressed(), dt)) {
            seq.currentTrack -= 1;
            drawNotes(recordedNotes[seq.currentTrack-1], seq);
            if (seq.currentTrack <= 1) {
                seq.currentTrack = 1;
            }
        }
        if (downKey.check(inputs.isDownPressed(), dt)) {
            seq.currentTrack += 1;
            drawNotes(recordedNotes[seq.currentTrack-1], seq);
            if (seq.currentTrack >= seq.NUM_TRACKS) {
                seq.currentTrack = seq.NUM_TRACKS;
            }
        }
    } else {
        if (upKey.check(inputs.isUpPressed(), dt)) {
            note_selection_vert += 1;
            drawNotes(recordedNotes[seq.currentTrack-1], seq);
            if (note_selection_vert >= 10) note_selection_vert = 10;
        }
        if (downKey.check(inputs.isDownPressed(), dt)) {
            note_selection_vert -= 1;
            drawNotes(recordedNotes[seq.currentTrack-1], seq);
            if (note_selection_vert <= 0) note_selection_vert = 0;
        }
    }

    if (fastKey.check(inputs.isBPressed(), dt)) {
        fastMovement = !fastMovement;
    }
}

void IndividualTrackUI::drawNotes(MidiRecording& recordedNotes, Sequencer& seq) {
    //ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p       = ImGui::GetCursorScreenPos();
    //ImVec2 wp      = ImGui::GetWindowPos();

    drawnNotes.clear();

    /*
        Top of notes: 0
        Bottom-most note: ppn*25
        Leftmost: 40
        Rightmost: 40 + ppb*(timeSigNum*4)
    */

    ImU32 white = IM_COL32(255,255,255,255);

    std::vector<double> x(26, -1.0f);

    float segmentOffset = -660.0f*current_segment;
    // Vertoffset is based on 0 as lowest note, with each difference being a distance of
    // ppn*7
    float verticalOffset = note_selection_vert*ppn*(-7.0f);

    /*
        Select which notes will be displayed here
        vert_selection = 0; 0-25
        vert_selection = 1; 7-32
    */
    int noteMin = note_selection_vert*7;
    int noteMax = noteMin+25;

    /*
    dl->AddRectFilled(ImVec2(0, 4*ppn), ImVec2(2*ppb+40, 5*ppn+ppn), white);
    dl->AddRectFilled(ImVec2(2*ppb+40, 0), ImVec2(2*ppb+ppb+40, 2*ppn), white);
    dl->AddRectFilled(ImVec2(3*ppb+40, 6*ppn), ImVec2(6*ppb+40, 7*ppn+ppn), white);
    dl->AddRectFilled(ImVec2(7*ppb+40, 20*ppn), ImVec2(10*ppb+40, 21*ppn+ppn), white);
    dl->AddRectFilled(ImVec2(11*ppb+40, 14*ppn), ImVec2(15*ppb+40, 15*ppn+ppn), white);
    */

    std::vector<TimedMidiMessage> events;
    if (recordedNotes.getLength().count() != 0) events = recordedNotes.getEvents();
    else {
        return;
    }
    // Equation for timestamp to beat conversion:
    // timestamp*0.1^6*seq.tempo/60
    double t = 1000000.0;
    
    double ttb = static_cast<double>(seq.tempo)/60.0; // timestamp to beats

    if (isMoving) {
        // Calculate maximum number of segments
        // Each segment is seq.time_sig_num*4, so max number of segments is full length/that value rounded up
        int segmentBeats = seq.timeSigNum*4;
        double segments = recordedNotes.getLength().count()/t*ttb/segmentBeats;
        if (segments >= 5.0) max_segments = int(segments) + 1;
        else max_segments = 4;
    }

    for (TimedMidiMessage& noteEvent : events) {
        int note = noteEvent.getNote().getMidiNote();
        if (note >= noteMin && note <= noteMax && noteEvent.isOn()) {
            // Save start x position
            x[note-noteMin] = 60.0f+segmentOffset+ noteEvent.getTimestamp().count()/t*ttb*ppb;
        }
        if (note >= noteMin && note <= noteMax && !noteEvent.isOn() && x[note-noteMin] != -1.0f) {
            // Draw note
            y = ((420-ppn) - (note-noteMin)*ppn);
            x_end = 60.0f+segmentOffset+ noteEvent.getTimestamp().count()/t*ttb*ppb;
            drawnNotes.push_back({ImVec2(p.x+x[note-noteMin],p.y+y), ImVec2(p.x+x_end,p.y+y+ppn)});
            //dl->AddRectFilled(ImVec2(x[note-noteMin],y), ImVec2(x_end,y+ppn),noteCol);
            x[note-noteMin] = -1.0f;
        }
    }
}

void IndividualTrackUI::pushCursorPlayback(Sequencer& seq, double dt, MidiRecording& recordedNotes) {
    double cursor_push = static_cast<double>(seq.tempo)/60.0*ppb;
    cursor_pos += cursor_push*dt;
    if (cursor_pos >= 720.0 && current_segment < max_segments-1) {
        cursor_pos = 60.0;
        current_segment += 1;
        drawNotes(recordedNotes, seq);
    } else if (cursor_pos >= 720.0 && current_segment >= max_segments - 1) {
        if (playback) isMoving = false;
        if (!playback) {
            max_segments += 1;
            current_segment += 1;
            cursor_pos = 60.0;
            drawNotes(recordedNotes, seq);
        }
    }
}