cmake_minimum_required(VERSION 3.14)
project(PoDAW)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-2.0.22
)
#set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL2)

# Use X11 on Linux
find_package(OpenGL REQUIRED COMPONENTS EGL GLX GLES2)
find_package(X11 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiodcxx)
#set(GLFW_USE_X11 ON CACHE BOOL "" FORCE)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_sdl2.h
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_sdlrenderer2.h
    imgui/backends/imgui_impl_sdlrenderer2.cpp
)

# Added stk subdirectory (already configured with a CMakeLists.txt) and audio surce code
add_subdirectory(stk)
#add_subdirectory(sdl)

set(AUDIO_CODE_SOURCES
    Instrument.cpp
    Piano.cpp
    Drums.cpp
    Guitar.cpp
    Bass.cpp
    AudioHandler.cpp
)

set(MIDI_CODE_SOURCES
    ${AUDIO_CODE_SOURCES}
    MidiHandler.cpp
    MidiRecorder.cpp
    MidiPlayer.cpp
)

set(STK_INCLUDE_DIRS
    stk/include
    stk/rawwaves
)

set(PODAW_SOURCES
    main.cpp
    Inputs.cpp
    Sequencer.cpp
    SettingsUI.cpp
    SequencerUI.cpp
    IndividualTrack.cpp
    GpioInputs.cpp
    #KeyboardInputs.cpp
)

# 3. Create Executables
add_executable(PoDAW_Test main.cpp ${IMGUI_SOURCES} ${PODAW_SOURCES} ${MIDI_CODE_SOURCES})

#add_executable(STK_Test stk_test.cpp ${AUDIO_CODE_SOURCES})

#add_executable(Audio_Live_Test audio_pipeline_test.cpp ${MIDI_CODE_SOURCES})

#add_executable(Audio_Recording_Test recording_test.cpp ${MIDI_CODE_SOURCES})

#add_executable(PoDAW_Test ${PODAW_SOURCES} ${IMGUI_SOURCES})

target_include_directories(PoDAW_Test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
    ${X11_INCLUDE_DIR}
    ${STK_INCLUDE_DIRS}
    #${AUDIO_CODE_SOURCES}
    #${MIDI_CODE_SOURCES}
)

#target_include_directories(STK_Test PRIVATE ${STK_INCLUDE_DIRS})

#target_include_directories(Audio_Live_Test PRIVATE ${STK_INCLUDE_DIRS})

#target_include_directories(Audio_Recording_Test PRIVATE ${STK_INCLUDE_DIRS})

# 5. Link Libraries (glfw now comes from the auto-download)
target_include_directories(PoDAW_Test PRIVATE ${GPIOD_INCLUDE_DIRS})
target_link_libraries(PoDAW_Test PRIVATE ${GPIOD_LIBRARIES})


#target_link_libraries(PoDAW_Test PRIVATE glfw OpenGL::GL OpenGL::EGL OpenGL::GLX OpenGL::GLES2 ${X11_LIBRARIES})
target_link_libraries(PoDAW_Test PRIVATE stk)
target_link_libraries(PoDAW_Test PRIVATE SDL2)

#target_link_libraries(STK_Test PRIVATE stk)

#target_link_libraries(Audio_Live_Test PRIVATE stk)

#target_link_libraries(Audio_Recording_Test PRIVATE stk)

#if(WIN32)
#    target_link_libraries(PoDAW_Test PRIVATE opengl32 gdi32)
#elseif(UNIX)
#    find_package(OpenGL REQUIRED)
#    target_link_libraries(PoDAW_Test PRIVATE OpenGL::GL)
#    if(PODAW_USE_GPIO)
#        target_link_libraries(PoDAW_Test PRIVATE gpiodcxx)
#    endif()
#endif()
