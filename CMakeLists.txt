cmake_minimum_required(VERSION 3.16)
project(RandomPicker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Raylib via FetchContent ────────────────────────────────────────────────────
include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
        GIT_SHALLOW    TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

add_executable(${PROJECT_NAME}
        main.cpp
        src/random.h
        src/screen.h
        src/MenuScreen.h
        src/MenuScreen.cpp
        src/NumberPicker.cpp
        src/NumberPicker.h
        src/Wheel.cpp
        src/Wheel.h
        src/Resources.h
        src/Dice.cpp
        src/Dice.h
        src/SlotMachine.cpp
        src/SlotMachine.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Copy fonts to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)


if (WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:-mwindows>
    )
endif()