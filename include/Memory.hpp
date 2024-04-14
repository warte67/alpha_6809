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
    public:
        Memory() { _deviceName = "Memory"; }
        Memory(std::string sName) : IDevice(sName) {}
        ~Memory() {};

        // unused virtuals
		void OnInit() override {}
		void OnQuit() override {}
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override {}
		void OnUpdate(float fElapsedTime) override {}
		void OnRender() override {}

        // pure virtuals
		Word OnAttach(Word nextAddr) override;

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        // public methods
        Word MemAlloc(Word size);   // returns address of the block in the extended heap
        Word MemDelete(Word addr);  // frees memory and returns number of bytes de-allocated
        Word MemAvailable();        // returns the number of bytes available in the heap

    private:

        Word reg_addr = 0;          // address register
        Word reg_pitch = 1;         // pitch register
        Word reg_width = 1;         // width register
        std::array<Byte, 65536> ext_memory = {0};

        Word reg_size = 0;          // dynamic memory size register
        Word reg_address = 0;       // dymamic memory address pointer
        Word reg_avail = 0;         // amount of available dynamic memory
        std::map<Word, Word> dyn_heap;    // first = address, second = size
        int memory_btm = 0x0000;    // current bottom of the memory heap

        // helpers
        Word _findFirstBlockOfSize(Word size);  // find an available block of SIZE
        void _onSizeLSB();          // fires when the LSB of MEM_DYN_SIZE is written to
        Word _memAvail();           // return the number of unallocated bytes on the heap
};


/**** NOTES *******************************************************************************
 * 
 *  Dynamic Memory Idea:
 *      SIZE        (Word) allocate on non-zero (LSB) write... free on zero (LSB) write
 *      ADDR        (Word) start address of allocated memory... start address to free
 *      FREE       (Word) returns the number of bytes available in the heap 
 * 
 *                      When SIZE reads as $0000 there was an allocation error
 *                      When writing $00 to the LSB of SIZE, the memory node pointed to 
 *                          by ADDR will be released. If there was an error freeing the 
 *                          memory, ADDR will report as $0000.
 *                      
 * 
 * 
 *  Options to include in the Gfx Device ???
 * 
 *  Extended Display Bitap Buffer:
 *      $0000-{required buffer size}    ; buffer size takes from the dynamic memory pool
 *  
 *      Extended Display uses the default display resolution as per the Gfx device.
 *      Number of bitplanes, however, may be increased if they fit.
 * 
 **** NOTES *******************************************************************************/