# MINIMUM CMAKE VERSION
cmake_minimum_required(VERSION 3.22)

# SET THE PROJECT NAME
set(PROJECT_NAME main)
project(${PROJECT_NAME})

# FIND THE SDL STUFF
find_package(SDL2 REQUIRED)

# SOURCE FILES
add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ./src/IDevice.cpp
    ./src/Bus.cpp
    ./src/Gfx.cpp
    ./src/Debug.cpp
    ./src/C6809.cpp
    ./src/Mouse.cpp
    ./src/Keyboard.cpp
    ./src/Gamepad.cpp
    ./src/FileIO.cpp
    ./src/Math.cpp
    ./src/MemBank.cpp
    ./src/Memory.cpp
)

# INCLUDE DIRECTORIES
include_directories(
    ${SDL2_INCLUDE_DIR}
    ./include)

# LINK LIBRARIES
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
)

# COMPILER FLAGS
target_compile_options(${PROJECT_NAME} PRIVATE 
    -fexceptions
    -Wall
    #-DDEBUG
)
