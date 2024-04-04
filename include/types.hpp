////
// types.hpp
//////

#pragma once


//#pragma comment(lib, "SDL2.lib")
//#pragma comment(lib, "SDL2main.lib")


#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <iostream>

#include "memory_map.hpp"
    
// Generate a memory map definition file?
#define COMPILE_MEMORY_MAP      false
#define MEMORY_MAP_OUTPUT_CPP   true

// simple 6809 types
    #ifndef Byte
        #define Byte Uint8
    #endif
    #ifndef Word
        #define Word Uint16
    #endif
    #ifndef DWord
        #define DWord Uint32
    #endif

  
// constants
constexpr Word VID_BUFFER_SIZE = 15 * 1024;      // standard video buffer size

constexpr Word DEBUG_WIDTH = 640;
constexpr Word DEBUG_HEIGHT = 360;
constexpr int DEBUG_BUFFER_WIDTH = DEBUG_WIDTH * 2;
constexpr int DEBUG_BUFFER_HEIGHT = DEBUG_HEIGHT * 2;
constexpr size_t DEBUG_BUFFER_SIZE = (DEBUG_WIDTH/8)*(DEBUG_HEIGHT/8);


constexpr bool DEBUG_SINGLE_STEP = false;




