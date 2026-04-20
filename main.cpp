#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <thread>

#include "Sequencer.hpp"
#include "SettingsUI.hpp"
#include "SequencerUI.hpp"
#include "IndividualTrack.hpp"
#include "KeyboardInputs.hpp"
#include "AudioHandler.hpp"
#include "MidiHandler.hpp"
#include "MidiPlayer.hpp"
#include "MidiRecording.hpp"
#include "MidiRecorder.hpp"
#include "Piano.hpp"
#include "Bass.hpp"
#include "Drums.hpp"
#include "Guitar.hpp"
#include "Inputs.hpp"

enum AppState { SEQUENCER, SETTINGS, INDIVIDUAL };

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Return boolean to use for isMoving
bool playbackToggle(double timestamp_position, IndividualTrackUI& individualUI,
                    std::vector<MidiRecording>& recordings, int selectedTrack) {
    // timestamp_position is in seconds, I don't know if you needed input timestamp to be in milli or microseconds so we can do the conversion here
    // microsecond conversion for example
    double timestamp_playback = timestamp_position * 1000000.0;
    // Add compatibility to check if app is already playing back, if so then playback can be stopped and cursor position reset
    // For now just make the app playback every single track, we'll see if single track playback is complex later

    //individualUI.isMoving = false; // if not playing
    //individualUI.isMoving = true; // if playing

    return false; // If not playing
    return true; // If playing
}

void recordToggle(MidiRecorder& recorder, MidiRecording& recording,
            double timestamp_pos, IndividualTrackUI& individualUI, Sequencer& seq, bool& isMoving) {
    if (!isMoving) {
        if (timestamp_pos <= 0) {
            // Add recording from timestamp 0 here
        } else {
            // Add recording from cursorPosition here 
        }
        isMoving = true;
    } else {
        // Stop recording here and return recording to 'recording' variable
        isMoving = false;
    }
}

int main(int, char**) {
    // Set MIDI 
    stk::Stk::setRawwavePath("../stk/rawwaves/");

    AudioHandler audioHandler;
    MidiRecorder recorder;
    MidiHandler midiHandler;
    MidiPlayer midiPlayer(audioHandler);

    MidiRecording track1Recording, track2Recording, track3Recording, track4Recording;
    std::vector<MidiRecording> recordings = {track1Recording, track2Recording, track3Recording, track4Recording};

    std::shared_ptr<Piano> piano = std::make_shared<Piano>();
    audioHandler.addInstrument(piano);
    recorder.setInstrument(piano);

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(480, 320, "PoDAW Interface", nullptr, nullptr);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);
    style.Colors[ImGuiCol_Header]   = ImVec4(0.30f, 0.50f, 0.55f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Sequencer     sequencer;
    SettingsUI    settingsUI;
    SequencerUI   sequencerUI;
    IndividualTrackUI individualUI;
    KeyboardInputs inputs(window);

    AppState currentState = SETTINGS;

    double cursorPosition = 0.0;

    bool isPlayback = true; // If playback option is selected = true, if recording = false
    bool isMoving = false; // GUI is moving if recording or playback

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(480, 320));
        ImGui::Begin("MainApp", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize     | ImGuiWindowFlags_NoSavedSettings);
        
        // isPlayback is true if playback option is chosen, false is recording option is chosen
        if (sequencer.currentMode == 0 && !isPlayback) {isPlayback=true; individualUI.playback = true;}
        else if (sequencer.currentMode == 1 && isPlayback) {isPlayback=false; individualUI.playback = false;}

        // Menu button is X
        if ((ImGui::IsKeyPressed(ImGuiKey_Escape) || inputs.isXPressed()) && !isMoving) {
            if (currentState == SETTINGS) {
                currentState = INDIVIDUAL;
                individualUI.drawNotes(recordings[sequencer.currentTrack-1], sequencer);
            } else {
                currentState = SETTINGS;
            }
        }
        // Playback/Record button is A for individual track viewer,
        // for individual track viewer B will toggle between fast movement
        if (currentState == INDIVIDUAL && inputs.isAPressed()){
            double timestamp_position = (cursorPosition-40.0)/individualUI.returnPPB()*60/sequencer.tempo;
            if (isPlayback) {
                isMoving = playbackToggle(timestamp_position, individualUI, recordings,
                                    sequencer.currentTrack);
            } else {
                recordToggle(recorder, recordings[sequencer.currentTrack-1],
                                timestamp_position, individualUI, sequencer, isMoving);
            }
        }  



        if (currentState == SETTINGS) {
            settingsUI.render(sequencer);
        } else if (currentState == SEQUENCER) {
            sequencerUI.render(sequencer, inputs, io.DeltaTime);
        } else if (currentState == INDIVIDUAL) {
            if (isMoving) {
                // individualUI.pushCursorPlayback(sequencer, ADD dt HERE);
                // I'm not entirely sure how playback works, where the current timestamp is stored, but if possible
                // I'd like to pass in the current dt each frame to have the cursor move consistently whether it is recording or playback
            }
            individualUI.render(sequencer, inputs, io.DeltaTime,
                                            recordings[sequencer.currentTrack-1],
                                            cursorPosition);
        }

        ImGui::End();

        ImGui::Render();
        int dw, dh;
        glfwGetFramebufferSize(window, &dw, &dh);
        glViewport(0, 0, dw, dh);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}