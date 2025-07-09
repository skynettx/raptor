cmake_minimum_required(VERSION 3.10)

set(EXECUTABLE_OUTPUT_PATH "bin")
set(CMAKE_CXX_FLAGS "-O3")
SET(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")
SET(CMAKE_C_STANDARD 11)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

project(raptor)

if(ANDROID)
    include(FetchContent)
    FetchContent_Declare(
        SDL
        URL            https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.10.zip
        SOURCE_DIR     ${CMAKE_BINARY_DIR}/include/SDL2
        )
    set(BUILD_SHARED_LIBS ON)
    set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
    set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
    set(SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL)
    set(SDL_ACTIVITY_FOLDER app/src/main/java/org/libsdl/app/)
    file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/android/${SDL_ACTIVITY_FOLDER})
    file(COPY ${CMAKE_BINARY_DIR}/include/SDL2/android-project/${SDL_ACTIVITY_FOLDER} DESTINATION ${CMAKE_SOURCE_DIR}/android/${SDL_ACTIVITY_FOLDER})
    file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/android/app/src/main/assets)
    file(COPY ${CMAKE_SOURCE_DIR}/include/TinySoundFont/LICENSE DESTINATION ${CMAKE_SOURCE_DIR}/android/app/src/main/assets/licenses)
    file(RENAME ${CMAKE_SOURCE_DIR}/android/app/src/main/assets/licenses/LICENSE ${CMAKE_SOURCE_DIR}/android/app/src/main/assets/licenses/LICENSE.TSF)
    file(COPY ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_SOURCE_DIR}/android/app/src/main/assets/licenses)
elseif(WIN32)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    if(MSVC)
        set(SDL2_DIR "include/SDL2-devel-2.28.2-VC/SDL2-2.28.2/cmake")
    else()
        set(SDL2_DIR "include/SDL2-devel-2.28.2-mingw/SDL2-2.28.2/cmake")
    endif()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

include_directories(./include/textscreen/include/)
include_directories(./include/TinySoundFont/)

add_subdirectory(./include/textscreen/include)

set(SOURCE_FILES
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
    src/movie_a.cpp
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
    src/mpuwinmm.cpp
    src/mputsf.cpp
    src/mpualsa.cpp
    src/mpucorea.cpp
    src/mpucorem.cpp
    )

set(SOURCE_SETUP_FILES
    src/setup/input.cpp
    src/setup/main.cpp
    src/setup/music.cpp
    src/setup/prefapi.cpp
    src/setup/sound.cpp
    src/setup/txt_joyinput.cpp
    src/setup/txt_keyinput.cpp
    src/setup/txt_mouseinput.cpp
    )

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_library( ${PROJECT_NAME} SHARED ${SOURCE_FILES})   
    target_link_libraries(raptor
        textscreen
        SDL2::SDL2
        )

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(ALSA REQUIRED)
    add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )
    target_link_libraries(raptor
        textscreen
        asound
        SDL2::SDL2main
        SDL2::SDL2
        )

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(AUDIO_TOOLBOX AudioToolbox)
    find_library(AUDIO_UNIT AudioUnit)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(COREMIDI_LIB CoreMIDI)
    add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )
    target_link_libraries(raptor ${AUDIO_TOOLBOX} ${AUDIO_UNIT} ${FOUNDATION_LIBRARY} ${COREMIDI_LIB}
        textscreen
        SDL2::SDL2main
        SDL2::SDL2
        )

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )
    target_sources(raptor PRIVATE rsrc/resource.rc)
    target_link_libraries(raptor
        winmm.lib
        textscreen
        SDL2::SDL2main
        SDL2::SDL2
        )
    if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} MATCHES "Release")
        if(MSVC)
            target_link_options(raptor PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
        else()
            target_link_options(raptor PRIVATE -mwindows)
        endif()
    elseif(NOT MSVC)
        target_link_options(raptor PRIVATE -mconsole)
    endif()
else()
    add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )
    target_link_libraries(raptor
        textscreen
        SDL2::SDL2main
        SDL2::SDL2
        )
endif()

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_executable( raptorsetup ${SOURCE_SETUP_FILES} )
    target_link_libraries(raptorsetup
        textscreen
        SDL2::SDL2main
        SDL2::SDL2
        )
    if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} MATCHES "Release")
        if(MSVC)
            target_link_options(raptorsetup PRIVATE /subsystem:windows /ENTRY:mainCRTStartup)
        elseif(WIN32)
            target_link_options(raptorsetup PRIVATE -mwindows)
        endif()
    elseif(WIN32 AND NOT MSVC)
        target_link_options(raptorsetup PRIVATE -mconsole)
    endif()
endif()
   
