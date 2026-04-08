#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// UI State Machine
enum AppState { SEQUENCER, SETTINGS, PLAYBACK_INDIVIDUAL };
AppState current_state = PLAYBACK_INDIVIDUAL;

// App Variables
int selected_menu = 0;
int current_track = 1;
int tempo = 180;
int time_signature_numerator = 4;
int time_signature_denominator = 4;

const int NUM_TRACKS = 4;
const int NUM_STEPS = 16;
bool grid_data[NUM_TRACKS][NUM_STEPS] = {false}; // Stores where notes are placed
int cursor_track = 0;
int cursor_step = 0;

// New variables
int NUM_NOTES_PER_CHUNK = 36;
int current_note_chunk_selection = 1;
int track_maximum = 5;
int current_track_chunk = 0;
int track_chunks_maximum = 1;


static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // Create a window sized for a standard embedded display (e.g., Raspberry Pi touchscreen)
    GLFWwindow* window = glfwCreateWindow(480, 320, "PoDAW Interface", nullptr, nullptr);
    //Fullscreen change//GLFWwindow* window = glfwCreateWindow(480, 320, "PoDAW Interface", glfwGetPrimaryMonitor(), nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Enable Keyboard/Gamepad Navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      

    // Setup ImGui style to match your mockup (Dark/Gray/Blue)
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f; // Blocky, hardware feel
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // Gray background
    style.Colors[ImGuiCol_Header] = ImVec4(0.4f, 0.6f, 0.6f, 1.0f);   // Muted teal for selection

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Check for our "Back/Menu" button toggle (Escape key for testing)
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            // Exit settings and redirect based on selected_menu
            if (current_state == SETTINGS) {
                if (selected_menu == 2) {
                    current_state = SEQUENCER;
                } else {
                    current_state = PLAYBACK_INDIVIDUAL;
                }
            }
            // Enter settings
            else {
                current_state = SETTINGS;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Lock the main window to the display size, remove title bar and borders
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(480, 320));
        ImGui::Begin("MainApp", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

        if (current_state == SETTINGS) {
            // --- SETTINGS PAGE MOCKUP ---
            // Left Menu Pane (30% width)
            ImGui::BeginChild("LeftMenu", ImVec2(140, 280), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened);
            if (ImGui::Selectable("Playback", selected_menu == 0)) selected_menu = 0;
            if (ImGui::Selectable("Recording", selected_menu == 1)) selected_menu = 1;
            if (ImGui::Selectable("All Tracks", selected_menu == 2)) selected_menu = 2;
            ImGui::EndChild();

            ImGui::SameLine();

            // Right Content Pane
            ImGui::BeginChild("RightContent", ImVec2(0, 280), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened);
            // Layout for Options
            ImGui::SetCursorPosY(50); // Push down a bit
            ImGui::SetCursorPosX(50);
            ImGui::Text("Tempo:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            // InputInt automatically maps to left/right or up/down when highlighted
            ImGui::InputInt("##tempo", &tempo, 1, 10); 
            
            ImGui::SetCursorPosY(120);
            ImGui::SetCursorPosX(50);
            ImGui::Text("Track:    ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            // Track number minimum
            if (current_track < 1) { current_track = 1; }
            // Track number maximum
            if (current_track > track_maximum) current_track = track_maximum;
            ImGui::InputInt("##current_track", &current_track, 1, 20);

            ImGui::SetCursorPosY(180);
            ImGui::SetCursorPosX(50);
            ImGui::Text("Time Sig(Top): ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            // Time signature minimum
            if (time_signature_numerator < 1) time_signature_numerator = 1;
            // Time signature maximum
            if (time_signature_numerator > 32) time_signature_numerator = 32;
            ImGui::InputInt("##time_signature_numerator", &time_signature_numerator, 1, 10);
            
            ImGui::SetCursorPosY(200);
            ImGui::SetCursorPosX(50);
            ImGui::Text("               --");

            ImGui::SetCursorPosY(220);
            ImGui::SetCursorPosX(50);
            ImGui::Text("Time Sig(Bot): ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            // Time signature minimum
            if (time_signature_denominator < 1) time_signature_denominator = 1;
            // Time signature maximum
            if (time_signature_denominator > 32) time_signature_denominator = 32;
            ImGui::InputInt("##time_signature_denominator", &time_signature_denominator, 1, 10);

            ImGui::EndChild();
        } 
        else if (current_state == SEQUENCER) {
            // 1. Handle D-Pad Inputs for the Cursor
            if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && cursor_track > 0) cursor_track--;
            if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && cursor_track < NUM_TRACKS - 1) cursor_track++;
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow) && cursor_step > 0) cursor_step--;
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow) && cursor_step < NUM_STEPS - 1) cursor_step++;
            
            // Handle Action Button (Toggle Note)
            if (ImGui::IsKeyPressed(ImGuiKey_Space) || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                grid_data[cursor_track][cursor_step] = !grid_data[cursor_track][cursor_step];
            }

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();
            
            // 2. Draw Left Track Header (Blue)
            draw_list->AddRectFilled(p, ImVec2(p.x + 80, p.y + 280), IM_COL32(70, 130, 180, 255));
            for (int i = 0; i < NUM_TRACKS; i++) {
                char track_label[16];
                snprintf(track_label, sizeof(track_label), "Track %d", i + 1);
                draw_list->AddText(ImVec2(p.x + 10, p.y + 20 + (i * 60)), IM_COL32(255, 255, 255, 255), track_label);
            }

            // 3. Draw the Grid and Notes
            float start_x = p.x + 90; // Start drawing after the header
            float start_y = p.y + 15;
            float cell_w = 22;        // Width of one step
            float cell_h = 30;        // Height of one track
            float spacing_y = 60;     // Space between tracks

            for (int t = 0; t < NUM_TRACKS; t++) {
                for (int s = 0; s < NUM_STEPS; s++) {
                    ImVec2 cell_min(start_x + (s * cell_w), start_y + (t * spacing_y));
                    ImVec2 cell_max(cell_min.x + cell_w - 2, cell_min.y + cell_h);
                    
                    // Draw placed notes (Blue blocks)
                    if (grid_data[t][s]) {
                        draw_list->AddRectFilled(cell_min, cell_max, IM_COL32(100, 150, 250, 255));
                    } else {
                        // Draw empty faint grid slots to guide the user
                        draw_list->AddRect(cell_min, cell_max, IM_COL32(100, 100, 100, 80));
                    }

                    // Draw the Cursor (Thick Yellow Box)
                    if (t == cursor_track && s == cursor_step) {
                        draw_list->AddRect(cell_min, cell_max, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f); 
                    }
                }
            }
        }
        else if (current_state == PLAYBACK_INDIVIDUAL) {
            // 1. Handle D-Pad Inputs for the Cursor
            // Which notes are being displayed

            // Use "current_note_chunk_selection" later once number of notes per track have been confirmed

            //if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && cursor_track > 0) cursor_track--;
            //if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && cursor_track < NUM_TRACKS - 1) cursor_track++;
            
            // Cursor location
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
                if (cursor_step > 0) cursor_step--;
                if (cursor_step <= 0 && current_track_chunk > 0) {
                    cursor_step = time_signature_numerator*4 - 1;
                    current_track_chunk--;
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
                if (cursor_step < time_signature_numerator*4 - 1) cursor_step++;
                if (cursor_step >= time_signature_numerator*4 - 1 && current_track_chunk < track_chunks_maximum) {
                    cursor_step = 0;
                    current_track_chunk++;
                }
            }  

            // Handle Input here

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();

            // 2. Draw Left Track detailer (Blue)
            draw_list->AddRectFilled(p, ImVec2(p.x + 40, p.y + 280), IM_COL32(70, 130, 180, 255));
            char track_num[16];
            snprintf(track_num, sizeof(track_num), "%d", current_track);
            draw_list->AddText(ImVec2(p.x + 10, p.y + 20), IM_COL32(255,255,255,255), track_num);
            // Next is track instrument
            snprintf(track_num, sizeof(track_num), "PNO");
            draw_list->AddText(ImVec2(p.x+10,p.y+30), IM_COL32(255,255,255,255), track_num);
            // Debug: Track chunk number
            snprintf(track_num, sizeof(track_num), "%d", current_track_chunk);
            draw_list->AddText(ImVec2(p.x+10,p.y+60), IM_COL32(255,255,255,255), track_num);
            
            // 3. Draw notes (For now just draw 3 octaves of notes, default 4/4 in a checkered pattern)
            /* Height: 320
                Width: 480
            
                Height of visible window: 280 
                Width of visible window: 440
                Height of each note: 280 / (Number of notes we want to display)
                Width of each beat: 440 / (Numerator of time signature * 4)
            */
           float note_height = 280/NUM_NOTES_PER_CHUNK;
           float note_width = 440/(time_signature_numerator*4);

           // Checkered pattern for chunk 1
           if (current_track_chunk == 0) {
                for (int i = 0; i < NUM_NOTES_PER_CHUNK; i++) {
                        for (int j = 0; j < (time_signature_numerator*4); j++) {
                            if ((i % 2 == 0 && (j + 1) % 2 == 0) || ((i+1) % 2 == 0 && j % 2 == 0)) {
                                draw_list->AddRectFilled(ImVec2(p.x+40 + note_width*j, p.y + 10 + note_height*i),
                                                        ImVec2(p.x+40 + note_width*(j+1), p.y + 10 + note_height*(i+1)),
                                                    IM_COL32(100,100,100,255));
                            }
                        }
                }
            }
           // Even pattern for chunk 2
           if (current_track_chunk == 1) {
                for (int i = 0; i < NUM_NOTES_PER_CHUNK; i++) {
                        for (int j = 0; j < (time_signature_numerator*4); j++) {
                            if ((i % 2 == 0 && (j) % 2 == 0) || ((i+1) % 2 == 0 && (j+1) % 2 == 0)) {
                                draw_list->AddRectFilled(ImVec2(p.x+40 + note_width*j, p.y + 10 + note_height*i),
                                                        ImVec2(p.x+40 + note_width*(j+1), p.y + 10 + note_height*(i+1)),
                                                    IM_COL32(100,100,100,255));
                            }
                        }
                }
            }

           // 4. Draw cursor
           // This "cursor" is different from the previous in that it's just one long line
           /*
                Math for cursor position:
                Start of cursor: 40
                End of cursor: Beginning of final note
                Position of cursor based on cursor_step:
                    40 + cursor_step*(note_width)
           */
          draw_list->AddRectFilled(ImVec2(p.x+40 + cursor_step*note_width, p.y),
                                    ImVec2(p.x+50 + cursor_step*note_width, p.y+470),
                                    IM_COL32(90,80,30,200));
        }

        // Draw Bottom Transport Bar (Yellow/Brownish)
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(win_pos.x, win_pos.y + 280), 
            ImVec2(win_pos.x + 480, win_pos.y + 320), 
            IM_COL32(100, 80, 30, 255)
        );

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}