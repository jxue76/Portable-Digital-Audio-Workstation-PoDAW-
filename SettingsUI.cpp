#include "SettingsUI.hpp"
#include "Sequencer.hpp"
#include "imgui.h"

void SettingsUI::render(Sequencer& seq) {
    ImGui::BeginChild("LeftMenu", ImVec2(140, 280),
                      ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened);
    if (ImGui::Selectable("Playback",  selectedMenu == 0)) selectedMenu = 0;
    if (ImGui::Selectable("Recording", selectedMenu == 1)) selectedMenu = 1;
    if (ImGui::Selectable("Options",   selectedMenu == 2)) selectedMenu = 2;
    if (ImGui::Selectable("Misc.",     selectedMenu == 3)) selectedMenu = 3;
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightContent", ImVec2(0, 280), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened);
    switch (selectedMenu) {
        case 0: renderPlayback(seq);  break;
        case 1: renderRecording(seq); break;
        case 2: renderOptions(seq);   break;
        case 3: renderMisc();         break;
    }
    ImGui::EndChild();
}

void SettingsUI::renderPlayback(Sequencer& seq) {
    seq.currentMode = 0;
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::Text("Tempo:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##tempo", &seq.tempo, 1, 10);
    if (seq.tempo < 20)  seq.tempo = 20;
    if (seq.tempo > 300) seq.tempo = 300;

    ImGui::SetCursorPosX(20);
    ImGui::Text("Time Sig:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##tsNum", &seq.timeSigNum, 1, 0);
    ImGui::SameLine();
    ImGui::Text("/");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##tsDen", &seq.timeSigDen, 1, 0);
    if (seq.timeSigNum < 1) seq.timeSigNum = 1;
    if (seq.timeSigNum > 32) seq.timeSigNum = 32;

    if (seq.timeSigDen < 1) seq.timeSigDen = 1;
    if (seq.timeSigDen > 32) seq.timeSigDen = 32;

    ImGui::SetCursorPosX(20);
    ImGui::Checkbox("Loop", &seq.looping);

    ImGui::SetCursorPosX(20);
    ImGui::Checkbox("Metronome", &seq.metronome);

    ImGui::SetCursorPosX(20);
    ImGui::Text("Selected Track: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##currentTrack", &seq.currentTrack, 1, 10);
    if (seq.currentTrack < 1) seq.currentTrack = 1;
    if (seq.currentTrack > seq.NUM_TRACKS) seq.currentTrack = seq.NUM_TRACKS;
}

void SettingsUI::renderRecording(Sequencer& seq) {
    seq.currentMode = 1;
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::TextWrapped("Incoming keyboard notes are converted into MIDI note events and recorded onto the armed track at the current playhead step.");

    ImGui::SetCursorPosX(20);
    int vel = static_cast<int>(seq.inputVelocity);
    ImGui::Text("Fixed Velocity: %d", vel);
    ImGui::SetCursorPosX(20);
    ImGui::SetNextItemWidth(180);
    if (ImGui::SliderInt("##vel", &vel, 1, 127)) {
        seq.inputVelocity = static_cast<uint8_t>(vel);
    }

    ImGui::SetCursorPosX(20);
    ImGui::Text("Last Note: %s", Sequencer::noteToName(seq.getLastRecordedNote()));

    ImGui::Spacing();
    ImGui::SetCursorPosX(20);
    ImGui::Text("MIDI Channels:");
    for (int i = 0; i < Sequencer::NUM_TRACKS; i++) {
        ImGui::SetCursorPosX(30);
        int ch = static_cast<int>(seq.tracks[i].midiChannel);
        char label[32];
        snprintf(label, sizeof(label), "%s Ch##%d", seq.tracks[i].name, i);
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputInt(label, &ch, 1, 1)) {
            if (ch < 0)  ch = 0;
            if (ch > 15) ch = 15;
            seq.tracks[i].midiChannel = static_cast<uint8_t>(ch);
        }
    }
}

void SettingsUI::renderOptions(Sequencer& seq) {
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::Text("Track Names:");
    for (int i = 0; i < Sequencer::NUM_TRACKS; i++) {
        ImGui::SetCursorPosX(30);
        char label[32];
        snprintf(label, sizeof(label), "##trackName%d", i);
        ImGui::SetNextItemWidth(150);
        ImGui::InputText(label, seq.tracks[i].name, sizeof(seq.tracks[i].name));
    }
}

void SettingsUI::renderMisc() {
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::Text("PoDAW - Portable Digital Audio Workstation");
    ImGui::SetCursorPosX(20);
    ImGui::TextDisabled("Version 0.1.0");

    ImGui::Spacing();
    ImGui::SetCursorPosX(20);
    ImGui::TextWrapped("Controls (PC):");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("Arrow up/down: arm track");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("Arrow left/right: move playhead while stopped");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("Space: play/stop transport");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("R: toggle record mode");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("Backspace/Delete: clear armed step");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("A W S E D F T G Y H U J K: piano keys");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("Z/X: keyboard octave down/up");
    ImGui::SetCursorPosX(30);
    ImGui::BulletText("Escape: settings/sequencer");
}
