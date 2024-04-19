// *************************************************
// *
// * Memory.hpp
// *
// ***********************************
#pragma once

#include <array>
#include <map>
#include "IDevice.hpp" 

class Memory : public IDevice
{
    friend class Gfx;   // for private access to 'ext_memory' and 'memory_btm'

    public:
        Memory() { _deviceName = "Memory"; }
        Memory(std::string sName) : IDevice(sName) {}
        ~Memory() {};

        // unused virtuals
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override {}
		void OnRender() override {}

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;
		void OnUpdate(float fElapsedTime) override;

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        // public methods
        Word MemAlloc(Word size);   // returns address of the block in the extended heap
        Word MemDelete(Word addr);  // frees memory and returns number of bytes de-allocated
        Word MemAvailable();        // returns the number of bytes available in the heap

        bool LoadBMP(const std::string& file, Byte bits_per_pixel=8, bool updatePalette = true);

    private:

        Byte reg_dsp_flags = 0;     // extended display flags register
        Word reg_addr = 0;          // address register
        Word reg_pitch = 1;         // pitch register
        Word reg_width = 1;         // width register
        std::array<Byte, 65536> ext_memory = {0};

        Word reg_size = 0;          // dynamic memory size register
        Word reg_address = 0;       // dymamic memory address pointer
        Word reg_avail = 0;         // amount of available dynamic memory
        std::map<Word, Word> dyn_heap;    // first = address, second = size
        int memory_btm = 0x0000;    // bottom of the memory heap (AKA display buffer size)

        // helpers
        Word _findFirstBlockOfSize(Word size);  // find an available block of SIZE
        void _onSizeLSB();          // fires when the LSB of MEM_DYN_SIZE is written to
        Word _memAvail();           // return the number of unallocated bytes on the heap
};


/**** NOTES *******************************************************************************
 * 
 *  Dynamic Memory:
 *      SIZE    (Word) allocate on non-zero (LSB) write... free on zero (LSB) write
 *      ADDR    (Word) start address of allocated memory... start address to free
 *      FREE    (Word) returns the number of bytes available in the heap 
 * 
 *               When SIZE reads as $0000 there was an allocation error
 *               When writing $00 to the LSB of SIZE, the memory node pointed to 
 *                   by ADDR will be released. If there was an error freeing the 
 *                   memory, ADDR will report as $0000.
 *       
 *                
 *  MEM_DSP_FLAGS
 *      bit 7:      0:standard graphics, 1:extended graphics
 *      bits 2-6:   reserved (future tilemap support)
 *      bits 0-1:   color depth: 0:2-color, 1:4-color, 2:16-color, 3:256-color
 * 
 *      Consider moving the MEM_DSP_FLAGS register to the Gfx Device Hardware Registers immediately
 *          following the GFX_MODE register.  
 * 
 *      Rename: 
 *          GFX_MODE        to GFX_STD_MODE     ; bit flags remain unchanged
 *          MEM_DSP_FLAGS   to GFX_EXT_MODE     ; bit flags remain unchanged
 * 
 * 
 *  Extended Video Buffer:
 *      This buffer should be allocated dynamically using the 'dyn_heap' according to required 
 *          size when the display mode is started. Remove the 'memory_btm' variable, simply make
 *          use of the extended heap memory system.
 * 
 *  Standard Video Buffer:
 *      Consider moving the standard text / bitmap buffer to extended memory to free up CPU accessable
 *          memory. Like the notes for the Extended Video Buffer, the Standard buffer should be 
 *          dynamically allocated on the heap according to the required size. 
 *  
 *  Dynamically Allocate the Display Buffers:
 *      Whenever the video mode changes, pixel resolution and/or color depth of either Standard or
 *          Extended Video Modes, both dynamically allocated buffers should be released and re-allocated
 *          according to their required size. The Standard Video Buffer should begin at $0000 and the
 *          Extended buffer should begin immediately following. (Only Extended buffer in Ext RAM).
 * 
 *      Personally, I like the standard buffer being in the CPU addressable space as it allows student
 *          programer to be exposed to both video memory systems. The Register Port style video memory
 *          addressing scheme is a system that is used in "modern" retro machines such as the Commander 64.
 *          The CPU addressable video memory is more akin to how the video hardware worked on the 8-bit 
 *          machines. Moving forwared, I believe the standard buffer will continue to be CPU addressable.
 *          The extended buffer will be located at address $0000 in the extended memory buffer.
 * 
 * Sprites and Map Tiles Should be Dynamically Allocated:
 *      Each image data entry should be dynamically allocated and given its own pointer to pass on
 *          to an indexed image display table.  
 * 
 * 
 *  GIMP saves all indexed BMP images as 256-color regarless of the palette size. Unused palette
 *      entries are set to WHITE. Care should be taken to load the proper image format into 
 *      memory based on color depth.
 * 
 * 
 * 
 **** NOTES *******************************************************************************/