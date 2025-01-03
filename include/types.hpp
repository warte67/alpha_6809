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
  
// Graphics System Device Constants:
constexpr Word VID_BUFFER_SIZE = 15 * 1024;      // standard video buffer size
constexpr int MAIN_MONITOR = 0;
constexpr int DEBUG_MONITOR = 0;

// Debug Device Constants:
constexpr Word DEBUG_WIDTH = 576;
constexpr Word DEBUG_HEIGHT = 296;
constexpr int DEBUG_WINDOW_WIDTH = DEBUG_WIDTH * 2;
constexpr int DEBUG_WINDOW_HEIGHT = DEBUG_HEIGHT * 2;
constexpr size_t DEBUG_BUFFER_SIZE = (DEBUG_WIDTH/8)*(DEBUG_HEIGHT/8);
constexpr bool DEBUG_STARTS_ACTIVE = false;
constexpr bool DEBUG_SINGLE_STEP = false;

// Mouse Device Constants:
constexpr bool ENABLE_SDL_MOUSE_CURSOR = true;  // when the SDL cursor is displayed, the hardware cursor is not

// Keyboard Device Constants:
// ...

// FileIO Device Constants:
constexpr Word FILEHANDLESMAX = 256;

// Paged memory file:
// const std::string PAGED_MEMORY_FILENAME = "/home/jay/Documents/GitHub/alpha_6809/build/usb_drive/paged.mem";
const std::string PAGED_MEMORY_FILENAME = "./build/usb_drive/paged.mem";
constexpr DWord PAGED_MEMORY_BANKSIZE = 8192;

// Testing
// const std::string TESTING_BITMAP = "/home/jay/Documents/GitHub/alpha_6809/build/scene_c.bmp";
const std::string TESTING_BITMAP = "./build/scene_c.bmp";


/***************************
  
 Notes:
    If we start running out of room to fit the hardware registers. Try reducing the EDIT_BUFFER_SIZE. The
    Keyboard line edit buffer is using the upper hardware register space. Another option may be to hide the
    line edit buffer behind a 'single byte out at time' type register system.


  
  *************************/
