cmake_minimum_required(VERSION 3.14)
project(PoDAW)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

set(PODAW_SOURCES
    main.cpp
    Inputs.cpp
    Sequencer.cpp
    SettingsUI.cpp
    SequencerUI.cpp
)

# Platform-specific input backend
if(PODAW_USE_GPIO)
    list(APPEND PODAW_SOURCES GpioInputs.cpp)
else()
    list(APPEND PODAW_SOURCES KeyboardInputs.cpp)
endif()

add_executable(PoDAW_Test ${PODAW_SOURCES} ${IMGUI_SOURCES})

target_include_directories(PoDAW_Test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

target_link_libraries(PoDAW_Test PRIVATE glfw)

if(WIN32)
    target_link_libraries(PoDAW_Test PRIVATE opengl32 gdi32)
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(PoDAW_Test PRIVATE OpenGL::GL)
    if(PODAW_USE_GPIO)
        target_link_libraries(PoDAW_Test PRIVATE gpiodcxx)
    endif()
endif()
