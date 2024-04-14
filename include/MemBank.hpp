// *************************************************
// *
// * MemBank.hpp
// *
// ***********************************
#pragma once

#include "IDevice.hpp" 

class MemBank : public IDevice
{
    public:
        MemBank() { _deviceName = "MemBank"; }
        MemBank(std::string sName) : IDevice(sName) {}
        ~MemBank() {};

        // structures used

        enum BANK_TYPE {
            RANDOM_ACCESS=0,    // standard random access memory (RAM)
            PERSISTANT,         // persistant RAM... data is loaded and saved
            READ_ONLY,          // loaded on boot... read only memory (ROM)
            BANK_TYPE_MAX       // Count of these enumerated types
        };
        struct BANK_NODE {
            BANK_TYPE type = BANK_TYPE::PERSISTANT;  
            DWord seek_pos;     // byte position of this bank data within the file
        };
        struct BANK_HEADER {
            char id_string[7] = {"M_BANK"};
            float version = 0.0f;
            Byte bank_1_index = 0;
            Byte bank_2_index = 1;
            Word exec_vector = 0;
            BANK_NODE bank_node[256] ;
            // Byte bank_data[256][8192];
        };

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;

        // unused virtuals
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override {}
		void OnUpdate(float fElapsedTime) override {}
		void OnRender() override {}

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        // public methods
        void set_bank_1_page(Byte page);
        void set_bank_2_page(Byte page);
        Byte get_bank_1_page()                  { return _bank_header.bank_1_index; }
        Byte get_bank_2_page()                  { return _bank_header.bank_2_index; }
        void set_bank_1_type(BANK_TYPE type)    { _bank_header.bank_node[get_bank_1_page()].type = type; }
        void set_bank_2_type(BANK_TYPE type)    { _bank_header.bank_node[get_bank_2_page()].type = type; }
        BANK_TYPE get_bank_1_type()             { return _bank_header.bank_node[get_bank_1_page()].type; }
        BANK_TYPE get_bank_2_type()             { return _bank_header.bank_node[get_bank_2_page()].type; }

    private:

        BANK_HEADER _bank_header;

        bool _fileExists(const std::string& filename);  // returns true if the file exists
        bool _newDefaultFile();         // create a new 'paged.mem' bank file if not exists
        bool _loadHeader();             // load the header info from the 'paged.mem' file
        bool _saveHeader();             // save the header info to the 'paged.mem' file
        FILE* _fopen(const std::string filename);       // open a file with error handling

};


/**** NOTES *******************************************************************************
 * 
 *  Interface: 
 *          set_bank_1_page(page_index)     // set bank ones page (0-255)
 *          set_bank_2_page(page_index)     // set bank twos page (0-255)
 *  
 *          set_bank_1_type(BANK_TYPE)      // set bank ones type to ROM, RAM, or PERSIST
 *          set_bank_2_type(BANK_TYPE)      // set bank ones type to ROM, RAM, or PERSIST
 * 
 **** NOTES *******************************************************************************/