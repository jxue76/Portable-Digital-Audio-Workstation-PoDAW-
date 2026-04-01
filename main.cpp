#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// UI State Machine
enum AppState { SEQUENCER, SETTINGS };
AppState current_state = SETTINGS;

// App Variables
int selected_menu = 2; // Default to "Options"
int current_track = 0;
int tempo = 180;

const int NUM_TRACKS = 4;
const int NUM_STEPS = 16;
bool grid_data[NUM_TRACKS][NUM_STEPS] = {false}; // Stores where notes are placed
int cursor_track = 0;
int cursor_step = 0;

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

    // Create a window sized for a standard embedded display (e.g., Raspberry Pi touchscreen)
    GLFWwindow* window = glfwCreateWindow(480, 320, "PoDAW Interface", nullptr, nullptr);
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
            current_state = (current_state == SEQUENCER) ? SETTINGS : SEQUENCER;
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
            if (ImGui::Selectable("Options", selected_menu == 2)) selected_menu = 2;
            if (ImGui::Selectable("misc.", selected_menu == 3)) selected_menu = 3;
            ImGui::EndChild();

            ImGui::SameLine();

            // Right Content Pane
            ImGui::BeginChild("RightContent", ImVec2(0, 280), ImGuiChildFlags_Borders);
            if (selected_menu == 2) {
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
                ImGui::Text("Track:       1");

                ImGui::SetCursorPosY(180);
                ImGui::SetCursorPosX(50);
                ImGui::Text("Time Sig:  4/4");
            }
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