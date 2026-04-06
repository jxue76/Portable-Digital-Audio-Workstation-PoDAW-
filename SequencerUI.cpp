#include "SequencerUI.hpp"
#include "imgui.h"
#include <algorithm>

static constexpr float HEADER_W  = 80.0f;
static constexpr float GRID_X0   = 90.0f;
static constexpr float GRID_Y0   = 10.0f;
static constexpr float CELL_W    = 22.0f;
static constexpr float CELL_H    = 40.0f;
static constexpr float TRACK_PAD = 55.0f;
static constexpr float BAR_Y     = 280.0f;
static constexpr float BAR_H     = 40.0f;
static constexpr float WIN_W     = 480.0f;

void SequencerUI::render(Sequencer& seq, Inputs& inputs, float dt) {
    handleInput(seq, inputs, dt);
    updatePlayback(seq, dt);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p       = ImGui::GetCursorScreenPos();
    ImVec2 wp      = ImGui::GetWindowPos();

    // --- Track Headers ---
    dl->AddRectFilled(p, ImVec2(p.x + HEADER_W, p.y + BAR_Y - 5), IM_COL32(50, 100, 140, 255));
    for (int t = 0; t < Sequencer::NUM_TRACKS; t++) {
        float ty = p.y + GRID_Y0 + t * TRACK_PAD + 12;
        const bool armed = t == seq.getArmedTrack();
        if (armed) {
            dl->AddRectFilled(
                ImVec2(p.x + 4, p.y + GRID_Y0 + t * TRACK_PAD),
                ImVec2(p.x + HEADER_W - 4, p.y + GRID_Y0 + t * TRACK_PAD + CELL_H),
                IM_COL32(130, 45, 45, 255));
        }
        dl->AddText(ImVec2(p.x + 8, ty),
                    armed ? IM_COL32(255, 235, 235, 255) : IM_COL32(220, 220, 220, 255),
                    seq.tracks[t].name);
    }

    // --- Step Grid ---
    float sx = p.x + GRID_X0;
    float sy = p.y + GRID_Y0;
    const int beatStride = std::max(1, Sequencer::NUM_STEPS / std::max(1, seq.timeSigNum));

    for (int t = 0; t < Sequencer::NUM_TRACKS; t++) {
        for (int s = 0; s < Sequencer::NUM_STEPS; s++) {
            ImVec2 cmin(sx + s * CELL_W, sy + t * TRACK_PAD);
            ImVec2 cmax(cmin.x + CELL_W - 2, cmin.y + CELL_H);

            if (seq.hasNote(t, s)) {
                const MidiNote& n = seq.getNote(t, s);
                float vn = n.velocity / 127.0f;
                uint8_t r = static_cast<uint8_t>(60  + vn * 80);
                uint8_t g = static_cast<uint8_t>(100 + vn * 100);
                uint8_t b = static_cast<uint8_t>(180 + vn * 75);
                dl->AddRectFilled(cmin, cmax, IM_COL32(r, g, b, 255));

                if (CELL_W >= 20) {
                    const char* nn = Sequencer::noteToName(n.note);
                    ImVec2 ts = ImGui::CalcTextSize(nn);
                    float tx = cmin.x + (CELL_W - 2 - ts.x) * 0.5f;
                    float tcy = cmin.y + (CELL_H - ts.y) * 0.5f;
                    dl->AddText(ImVec2(tx, tcy), IM_COL32(255, 255, 255, 220), nn);
                }
            } else {
                bool beatLine = (s % beatStride) == 0;
                ImU32 borderCol = beatLine ? IM_COL32(120, 120, 120, 120)
                                           : IM_COL32(80, 80, 80, 60);
                dl->AddRect(cmin, cmax, borderCol);
            }
        }
    }

    // --- Playhead / current record column ---
    {
        int cs = seq.getCurrentStep();
        float phx = sx + cs * CELL_W;
        dl->AddRectFilled(
            ImVec2(phx, sy),
            ImVec2(phx + CELL_W - 2, sy + Sequencer::NUM_TRACKS * TRACK_PAD),
            seq.getMode() == Sequencer::Mode::RECORDING
                ? IM_COL32(255, 80, 80, 40)
                : IM_COL32(255, 255, 255, 30)
        );
        dl->AddLine(
            ImVec2(phx, sy),
            ImVec2(phx, sy + Sequencer::NUM_TRACKS * TRACK_PAD),
            seq.getMode() == Sequencer::Mode::RECORDING
                ? IM_COL32(255, 100, 100, 170)
                : IM_COL32(255, 255, 255, 120),
            2.0f
        );
    }

    // --- Transport Bar ---
    dl->AddRectFilled(
        ImVec2(wp.x, wp.y + BAR_Y),
        ImVec2(wp.x + WIN_W, wp.y + BAR_Y + BAR_H),
        IM_COL32(35, 35, 40, 255)
    );

    float barTextY = wp.y + BAR_Y + 12;

    switch (seq.getMode()) {
        case Sequencer::Mode::STOPPED:
            dl->AddRectFilled(
                ImVec2(wp.x + 10, barTextY - 2),
                ImVec2(wp.x + 22, barTextY + 14),
                IM_COL32(160, 160, 160, 255));
            dl->AddText(ImVec2(wp.x + 28, barTextY), IM_COL32(160, 160, 160, 255), "STOP");
            break;
        case Sequencer::Mode::PLAYING:
            dl->AddTriangleFilled(
                ImVec2(wp.x + 10, barTextY - 2),
                ImVec2(wp.x + 10, barTextY + 14),
                ImVec2(wp.x + 24, barTextY + 6),
                IM_COL32(80, 220, 80, 255));
            dl->AddText(ImVec2(wp.x + 28, barTextY), IM_COL32(80, 220, 80, 255), "PLAY");
            break;
        case Sequencer::Mode::RECORDING:
            dl->AddCircleFilled(
                ImVec2(wp.x + 16, barTextY + 6), 7,
                IM_COL32(230, 50, 50, 255));
            dl->AddText(ImVec2(wp.x + 28, barTextY), IM_COL32(230, 50, 50, 255), "REC");
            break;
    }

    char info[96];
    snprintf(info, sizeof(info), "%d BPM  |  step %d / %d  |  arm %s",
             seq.tempo, seq.getCurrentStep() + 1, Sequencer::NUM_STEPS,
             seq.tracks[seq.getArmedTrack()].name);
    dl->AddText(ImVec2(wp.x + 90, barTextY), IM_COL32(200, 200, 200, 255), info);

    const char* inputLabel = seq.hasLiveInputNote()
        ? Sequencer::noteToName(seq.getLiveInputNote())
        : Sequencer::noteToName(seq.getLastRecordedNote());

    char noteInfo[96];
    snprintf(noteInfo, sizeof(noteInfo), "%s: %s  vel:%d",
             seq.hasLiveInputNote() ? "live" : "last",
             inputLabel,
             seq.inputVelocity);
    ImVec2 niSize = ImGui::CalcTextSize(noteInfo);
    dl->AddText(ImVec2(wp.x + WIN_W - niSize.x - 12, barTextY),
                IM_COL32(200, 200, 200, 255), noteInfo);
}

void SequencerUI::handleInput(Sequencer& seq, Inputs& inputs, float dt) {
    if (upKey.check(inputs.isUpPressed(), dt)) {
        seq.setArmedTrack(seq.getArmedTrack() - 1);
    }
    if (downKey.check(inputs.isDownPressed(), dt)) {
        seq.setArmedTrack(seq.getArmedTrack() + 1);
    }
    if (seq.getMode() == Sequencer::Mode::STOPPED) {
        if (leftKey.check(inputs.isLeftPressed(), dt)) {
            seq.movePlayhead(-1);
        }
        if (rightKey.check(inputs.isRightPressed(), dt)) {
            seq.movePlayhead(1);
        }
    }

    bool aNow = inputs.isAPressed();
    if (aNow && !prevA) {
        seq.togglePlayback();
    }
    prevA = aNow;

    bool bNow = inputs.isBPressed();
    if (bNow && !prevB) {
        seq.clearStep(seq.getArmedTrack(), seq.getCurrentStep());
    }
    prevB = bNow;

    bool xNow = inputs.isXPressed();
    if (xNow && !prevX) {
        seq.toggleRecording();
    }
    prevX = xNow;

    MidiInputEvent event;
    while (inputs.pollMidiInputEvent(event)) {
        seq.recordMidiEvent(event);
    }
}

void SequencerUI::updatePlayback(Sequencer& seq, float dt) {
    if (seq.getMode() == Sequencer::Mode::STOPPED) {
        stepTimer = 0.0f;
        return;
    }

    float stepsPerBeat = static_cast<float>(Sequencer::NUM_STEPS) / seq.timeSigNum;
    float stepInterval = 60.0f / (seq.tempo * stepsPerBeat);

    stepTimer += dt;
    while (stepTimer >= stepInterval) {
        stepTimer -= stepInterval;
        seq.advanceStep();
    }
}
