#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>

#include "Sequencer.hpp"
#include "SettingsUI.hpp"
#include "SequencerUI.hpp"
#include "IndividualTrack.hpp"
#include "KeyboardInputs.hpp"
#include "GpioInputs.hpp"
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
#include "Midi_Recording_Utils.hpp"

enum AppState { SEQUENCER, SETTINGS, INDIVIDUAL };

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Return boolean to use for isMoving
/*bool playbackToggle(double timestamp_position, IndividualTrackUI& individualUI,
                    std::vector<MidiRecording>& recordings, int selectedTrack, MidiPlayer& midiPlayer) {
    // timestamp_position is in seconds, I don't know if you needed input timestamp to be in milli or microseconds so we can do the conversion here
    // microsecond conversion for example
    //double timestamp_playback = timestamp_position * 1000000.0;
    // Add compatibility to check if app is already playing back, if so then playback can be stopped and cursor position reset
    // For now just make the app playback every single track, we'll see if single track playback is complex later

    if (!midiPlayer.isPlaying()) {
        return true;
    } else {
        return false;
    }

    return false; // If not playing
    return true; // If playing
}*/

int main(int, char**) {
    // Set MIDI 
    stk::Stk::setRawwavePath("../stk/rawwaves/");

    // All instruments
    std::shared_ptr<Piano> piano = std::make_shared<Piano>("PNO");
    std::shared_ptr<Guitar> guitar = std::make_shared<Guitar>("GTR");
    std::shared_ptr<Drums> drums = std::make_shared<Drums>("DRM");
    std::shared_ptr<Bass> bass = std::make_shared<Bass>("BSS");

    MidiRecorder recorder;

    AudioHandler audioHandler;
    audioHandler.addInstrument(piano);
    audioHandler.addInstrument(guitar);
    audioHandler.addInstrument(drums);
    audioHandler.addInstrument(bass);

    MidiPlayer midiPlayerPiano(audioHandler);
    MidiPlayer midiPlayerGuitar(audioHandler);
    MidiPlayer midiPlayerDrums(audioHandler);
    MidiPlayer midiPlayerBass(audioHandler);

    //MidiRecording track1Recording, track2Recording, track3Recording, track4Recording;
    std::vector<MidiRecording> recordings(4);
    recordings[0].setInstrument(piano);
    recordings[1].setInstrument(guitar);
    recordings[2].setInstrument(drums);
    recordings[3].setInstrument(bass);

    /*recorder.setInstrument(guitar);

    std::vector<TestMidiHandler::ScheduledMidiMessage> schedule = {
        { std::chrono::milliseconds(1000), MidiMessage(Note(60, 1.0f), true) },
        { std::chrono::milliseconds(3000), MidiMessage(Note(60, 1.0f), false) },
        { std::chrono::milliseconds(5000), MidiMessage(Note(64, 1.0f), true) },
        { std::chrono::milliseconds(7000), MidiMessage(Note(64, 1.0f), false) },
        { std::chrono::milliseconds(9000), MidiMessage(Note(67, 1.0f), true) },
        { std::chrono::milliseconds(11000), MidiMessage(Note(67, 1.0f), false) }
    };

    TestMidiHandler testMidi(schedule);
    recorder.start();

    while (recorder.getElapsedTime() < std::chrono::seconds(12)) {
        testMidi.update();
        if (testMidi.hasMessages()) {
            MidiMessage msg = testMidi.popMessage();
            std::cout << "Test MIDI message: Note " << msg.getNote().getMidiNote() 
                      << (msg.isOn() ? " ON" : " OFF") << std::endl;
            recorder.process(msg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    recordings[1] = recorder.stop();*/


    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    //GLFWwindow* window = glfwCreateWindow(480, 320, "PoDAW Interface", nullptr, nullptr);
    //float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow(720, 480, "PoDAW", glfwGetPrimaryMonitor(), nullptr);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.FontGlobalScale = 1.5;

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
    GpioInputs inputs;
    //KeyboardInputs inputs(window);

    AppState currentState = SETTINGS;

    double cursorPosition = 0.0;

    bool isPlayback = true; // If playback option is selected = true, if recording = false
    bool isMoving = false; // GUI is moving if recording or playback

    auto beginning = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end-beginning);

    auto input_delay = std::chrono::high_resolution_clock::now();
    auto current_input_delay = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - input_delay);
    bool input_lock = false;

    TestMidiHandler midiHandler;

    //std::cout << "start of render" <<std::endl;
    bool input = false;

    while (true) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(30, 0));
        ImGui::SetNextWindowSize(ImVec2(720, 480));
        //ImGui::SetCursorPos(ImVec2(30,0));
        ImGui::Begin("MainApp", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);

        // isPlayback is true if playback option is chosen, false is recording option is chosen
        if (sequencer.currentMode == 0 && !isPlayback) {isPlayback=true; individualUI.playback = true;}
        else if (sequencer.currentMode == 1 && isPlayback) {isPlayback=false; individualUI.playback = false;}

        // Input lock code
        if (input_lock) {
            current_input_delay = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - input_delay);
            if (current_input_delay.count() > 0.01) {
                input_lock = false;
                input = false;
            }
        }

        if (currentState == SETTINGS && !input_lock) {
            if (inputs.isAPressed()) {
                io.AddKeyEvent(ImGuiKey_Enter, true);
                io.AddKeyEvent(ImGuiKey_Enter, false);
                input = true;
            } else if (inputs.isUpPressed()) {
                io.AddKeyEvent(ImGuiKey_UpArrow, true);
                io.AddKeyEvent(ImGuiKey_UpArrow, false);
                input = true;
            } else if (inputs.isDownPressed()) {
                io.AddKeyEvent(ImGuiKey_DownArrow, true);
                io.AddKeyEvent(ImGuiKey_DownArrow, false);
                input = true;
            } else if (inputs.isLeftPressed()) {
                io.AddKeyEvent(ImGuiKey_LeftArrow, true);
                io.AddKeyEvent(ImGuiKey_LeftArrow, false);
                input = true;
            } else if (inputs.isRightPressed()) {
                io.AddKeyEvent(ImGuiKey_RightArrow, true);
                io.AddKeyEvent(ImGuiKey_RightArrow, false);
                input = true;
            }
            if (input) {
                input_delay = std::chrono::high_resolution_clock::now();
                input_lock = true;
            }
        }

        if (inputs.getDialPosition() == Dial::UP && !input_lock) {
            if (piano->getVolume() < 1.0f) {
                piano->setVolume((piano->getVolume()) + 0.02f);
                guitar->setVolume((guitar->getVolume()) + 0.02f);
                drums->setVolume((drums->getVolume()) + 0.02f);
                bass->setVolume((bass->getVolume()) + 0.02f);
            }
            std::cout << piano->getVolume() - 0.02f << std::endl;
            //input_delay = std::chrono::high_resolution_clock::now();
            //input_lock = true;
        } else if (inputs.getDialPosition() == Dial::DOWN && !input_lock) {
            if (piano->getVolume() > 0.0f) {
                piano->setVolume((piano->getVolume()) - 0.02f);
                guitar->setVolume((guitar->getVolume()) - 0.02f);
                drums->setVolume((drums->getVolume()) - 0.02f);
                bass->setVolume((bass->getVolume()) - 0.02f);
            }
            std::cout << piano->getVolume() - 0.02f << std::endl;
            //input_delay = std::chrono::high_resolution_clock::now();
            //input_lock = true;
        }

        // Menu button is X
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            break;
        }
        if (inputs.isXPressed() && !isMoving && !input_lock) {
            if (currentState == SETTINGS) {
                currentState = INDIVIDUAL;
                individualUI.drawNotes(recordings[sequencer.currentTrack-1], sequencer);
            } else {
                currentState = SETTINGS;
            }
            input_delay = std::chrono::high_resolution_clock::now();
            input_lock = true;
        }
        // Playback/Record button is A for individual track viewer,
        // for individual track viewer B will toggle between fast movement
        if (currentState == INDIVIDUAL && inputs.isAPressed() && !input_lock){
            double timestamp_position = (cursorPosition-60.0)/individualUI.returnPPB()*60/sequencer.tempo;
            // seconds
            
            if (isPlayback) {
                //isMoving = playbackToggle(timestamp_position, individualUI, recordings,sequencer.currentTrack, midiPlayer);
                if (!isMoving) {
                    if (recordings[0].getEvents().size() > 0) midiPlayerPiano.play(recordings[0],false, timestamp_position);
                    if (recordings[1].getEvents().size() > 0) midiPlayerGuitar.play(recordings[1],false, timestamp_position);
                    if (recordings[2].getEvents().size() > 0) midiPlayerDrums.play(recordings[2],false, timestamp_position);
                    if (recordings[3].getEvents().size() > 0) midiPlayerBass.play(recordings[3],false, timestamp_position);
                    isMoving = true;
                } else {
                    midiPlayerPiano.stop();
                    midiPlayerGuitar.stop();
                    midiPlayerDrums.stop();
                    midiPlayerBass.stop();
                    isMoving = false;
                }
            } else {
                switch (sequencer.currentTrack) {
                    case 1:
                        recorder.setInstrument(piano);
                        break;
                    case 2:
                        recorder.setInstrument(guitar);
                        break;
                    case 3:
                        recorder.setInstrument(drums);
                        break;
                    case 4:
                        recorder.setInstrument(bass);
                        break;
                    default:
                        recorder.setInstrument(piano);
                }
                //recordToggle(recorder, recordings[sequencer.currentTrack-1],timestamp_position, individualUI, sequencer, isMoving);
                if (isMoving) {
                    recordings[sequencer.currentTrack-1] = recorder.stop();
                    isMoving = false;
                } else {
                    individualUI.setCursor(60.0f);
                    recorder.start();
                    isMoving = true;
                }
            }
            input_delay = std::chrono::high_resolution_clock::now();
            input_lock = true;
        }  

        // Recording loop
        if (!isPlayback && isMoving) {
            if (midiHandler.hasMessages() && !input_lock) {
                MidiMessage msg = midiHandler.popMessage();
                std::cout << "Received MIDI message: Note " << msg.getNote().getMidiNote() 
                        << (msg.isOn() ? " ON" : " OFF") << std::endl;
                if (msg.isOn()) {
                    switch (sequencer.currentTrack) {
                        case 1:
                            audioHandler.addNoteToInstrument(piano, msg.getNote());
                            break;
                        case 2:
                            audioHandler.addNoteToInstrument(guitar, msg.getNote());
                            break;
                        case 3:
                            audioHandler.addNoteToInstrument(drums, msg.getNote());
                            break;
                        case 4:
                            audioHandler.addNoteToInstrument(bass, msg.getNote());
                            break;
                        default:
                            audioHandler.addNoteToInstrument(piano, msg.getNote());
                    }
                } else {
                    switch (sequencer.currentTrack) {
                        case 1:
                            audioHandler.removeNoteFromInstrument(piano, msg.getNote());
                            break;
                        case 2:
                            audioHandler.removeNoteFromInstrument(guitar, msg.getNote());
                            break;
                        case 3:
                            audioHandler.removeNoteFromInstrument(drums, msg.getNote());
                            break;
                        case 4:
                            audioHandler.removeNoteFromInstrument(bass, msg.getNote());
                            break;
                        default:
                            audioHandler.removeNoteFromInstrument(piano, msg.getNote());
                    }
                }
                recorder.process(msg);
                //input_lock = true;
                //input_delay = std::chrono::high_resolution_clock::now();
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (isPlayback &&
            !midiPlayerPiano.isPlaying() &&
            !midiPlayerGuitar.isPlaying() &&
            !midiPlayerDrums.isPlaying() &&
            !midiPlayerBass.isPlaying()) isMoving = false;

        if (currentState == SETTINGS) {
            settingsUI.render(sequencer);
        } else if (currentState == SEQUENCER) {
            sequencerUI.render(sequencer, inputs, io.DeltaTime);
        } else if (currentState == INDIVIDUAL) {
            if (isMoving) {
                // individualUI.pushCursorPlayback(sequencer, ADD dt HERE);
                // I'm not entirely sure how playback works, where the current timestamp is stored, but if possible
                // I'd like to pass in the current dt each frame to have the cursor move consistently whether it is recording or playback
                individualUI.pushCursorPlayback(sequencer, io.DeltaTime, recordings[sequencer.currentTrack-1]);
                individualUI.isMoving = true;
            } else {
                individualUI.isMoving = false;
            }
            individualUI.render(sequencer, inputs, io.DeltaTime,
                                            recordings[sequencer.currentTrack-1],
                                            cursorPosition);
        }

        //std::cout << "End of render" << std::endl;

        ImGui::End();

        //style.ScaleAllSizes(1.1f);
        
        ImGui::Render();
        int dw, dh;
        glfwGetFramebufferSize(window, &dw, &dh);
        glViewport(0, 0, dw, dh);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        

        //glfwSetWindowSize(window, 900, 400);

        glfwSwapBuffers(window);
    }

    std::cout << "The end " << std::endl;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}