cmake_minimum_required(VERSION 3.14)
project(PoDAW)

set(CMAKE_CXX_STANDARD 17)

# 1. Auto-download GLFW in the background
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
# Turn off GLFW extras to speed up the build
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 2. Define ImGui Sources
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

# 3. Create Executable
add_executable(PoDAW_Test main.cpp ${IMGUI_SOURCES})

# 4. Include Directories
target_include_directories(PoDAW_Test PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

# 5. Link Libraries (glfw now comes from the auto-download)
target_link_libraries(PoDAW_Test PRIVATE glfw opengl32 gdi32)