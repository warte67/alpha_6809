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
constexpr Word VID_BUFFER_SIZE = 11 * 1024;      // standard video buffer size


//    const bool ENABLE_MOUSE_CURSOR = true;      // mouse cursor shown by default
//    const bool ENABLE_DEBUG = false;
//    const bool DEBUG_SINGLE_STEP = ENABLE_DEBUG;	// false;
//    
//    
//    constexpr Word EDIT_BUFFER_SIZE = 256;          // input/edit buffer size
//    
//    
//    // debugging
//    //const bool DEBUG_SCANTYPE_OLD = false;
