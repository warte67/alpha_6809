// *************************************************
// *
// * Memory.hpp
// *
// ***********************************
#pragma once

#include "IDevice.hpp" 

class Memory : public IDevice
{
    public:
        Memory() { _deviceName = "Memory"; }
        Memory(std::string sName) : IDevice(sName) {}
        ~Memory() {};

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnEvent(SDL_Event* evnt) override;
		void OnUpdate(float fElapsedTime) override;
		void OnRender() override;

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;
};


/**** NOTES *******************************************************************************
 * 
 *  MemBank Interface: 
 * 
 *      enum BANK_TYPE 
 *      {
 *          INVALID = 0,        // unused bank type
 *          RAM_VOLITILE,       // standard random access memory (RAM)
 *          RAM_PERSIST,        // persistant RAM... data is loaded and saved
 *          READ_ONLY,          // loaded on boot... read only memory (ROM)
 *          BANK_TYPE_MAX       // Count of these enumerated types
 *      };
 * 
 *      set_bank_1_page(page_index)     // set bank ones page (0-255)
 *      set_bank_2_page(page_index)     // set bank twos page (0-255)
 *
 *      set_bank_1_type(BANK_TYPE)      // set bank ones type to ROM, RAM, or PERSIST
 *      set_bank_2_type(BANK_TYPE)      // set bank ones type to ROM, RAM, or PERSIST
 * 
 **** NOTES *******************************************************************************/