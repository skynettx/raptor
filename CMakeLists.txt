cmake_minimum_required(VERSION 3.10)

set(EXECUTABLE_OUTPUT_PATH "bin")
set(CMAKE_CXX_FLAGS "-O3")
SET(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")
SET(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

project(raptor)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(./src/)
include_directories(./include/textscreen/include/)
include_directories(./include/TinySoundFont/)

add_subdirectory(./include/textscreen/include)
add_executable(${CMAKE_PROJECT_NAME}
    src/tile.cpp
    src/gssapi.cpp
    src/objects.cpp
    src/tile_a.cpp
    src/windows.cpp
    src/flame.cpp
    src/i_lastscr.cpp
    src/joyapi.cpp
    src/kbdapi.cpp
    src/glbapi.cpp
    src/i_video.cpp
    src/musapi.cpp
    src/movia_a.cpp
    src/ptrapi_a.cpp
    src/swdapi.cpp
    src/imsapi.cpp
    src/store.cpp
    src/i_oplmusic.cpp
    src/bonus.cpp
    src/anims.cpp
    src/shadow.cpp
    src/movie.cpp
    src/m_misc.cpp
    src/shots.cpp
    src/opl3.cpp
    src/eshot.cpp
    src/gfxapi_a.cpp
    src/dspapi.cpp
    src/vmemapi.cpp
    src/rap.cpp
    src/demo.cpp
    src/prefapi.cpp
    src/gfxapi.cpp
    src/intro.cpp
    src/help.cpp
    src/input.cpp
    src/fx.cpp
    src/ptrapi.cpp
    src/loadsave.cpp
    src/enemy.cpp
    src/mpuapi.cpp
    src/mputsf.cpp
    src/mpualsa.cpp
    src/mpucorea.cpp
    src/mpucorem.cpp
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(ALSA REQUIRED)
    target_link_libraries(raptor
        textscreen
        asound
        SDL2::SDL2
        SDL2::SDL2main
        )

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(AUDIO_TOOLBOX AudioToolbox)
    find_library(AUDIO_UNIT AudioUnit)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(COREMIDI_LIB CoreMIDI)
    target_link_libraries(raptor ${AUDIO_TOOLBOX} ${AUDIO_UNIT} ${FOUNDATION_LIBRARY} ${COREMIDI_LIB}
        textscreen
        SDL2::SDL2
        SDL2::SDL2main
        )
else()
    target_link_libraries(raptor
        textscreen
        SDL2::SDL2
        SDL2::SDL2main
        )
endif()
