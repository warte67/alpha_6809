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
            NOT_ASSIGNED = 0,   // unused bank type
            RAM_VOLITILE,       // standard random access memory (RAM)
            RAM_PERSIST,        // persistant RAM... data is loaded and saved
            READ_ONLY,          // loaded on boot... read only memory (ROM)
            BANK_TYPE_MAX       // Count of these enumerated types
        };
        struct BANK_NODE {
            BANK_TYPE type = BANK_TYPE::RAM_VOLITILE;  
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

    private:

        BANK_HEADER _bank_header;

        bool _fileExists(const std::string& filename);  // returns true if the file exists
        bool _newDefaultFile();         // create a new 'paged.mem' bank file if not exists
        bool _loadHeader();             // load the header info from the 'paged.mem' file
        bool _saveHeader();             // save the header info to the 'paged.mem' file

};


/**** NOTES ********************************************************************


    FILE STRUCTURE:
        HEADER "M_BANK\0" (char [7])    ; null-terminated file identifier string
        RAW_FLOAT (4 Bytes)             ; RAW float representing the file version
        BANK_1_INDEX (1 Byte)           ; default index for bank one
        BANK_2_INDEX (1 Byte)           ; default index for bank two
        EXEC_VECTOR                     ; cpu address to begin code execution
                                        ;   if zero, do not autostart 
        BANK_NODE[256]                  ; an array of BANK_NODE structures defining 
                                        ;   the stored bank data.
        BANK_DATA x BANK_COUNT          ; the stored bank data 

    BANK_TYPE
        NOT_ASSIGNED    ; unused memory bank
        RAM             ; Standard Random Access Memory (RAM). Zero's out on boot.
        RAM_PERSIST     ; Persistent RAM. Data is loaded on boot. Saved on quit.
        ROM             ; Loaded on boot. Read Only Memory (ROM).
    BANK_NODE
        BANK_TYPE,
        SEEK_POSITION   ; (DWord) File Seek position of the start of this bank
                        ; (all banks are 8K in size)

    While the 6809 is quite capable of delivering completely position independant 
    code, care should be taken to position program code into the bank space for 
    which it was originally designed by distributing it within an odd or even bank:
        Odd indexed banks should use bank ONE address space ($B000-$CFFF).
        Even indexed banks should use bank TWO address space ($D000-$EFFF).

    As a rule, all code banks should begin execution from the first address of their
    bank. Use a JMP {addr} at the start of the bank if needed. If the first byte of
    a loaded memory bank is a NULL (NEG {direct}), it is asumed to be non-executable.

    On baremetal devices, the memory bank file associations are to be stored on
    the USB storage media. The PI PICOs are each limited to 2MB on-board flash
    which should be solely used for firmware. The USB drives will be used as the
    computers "hard drive(s)." Associating a file with RAM allows for persistent 
    storage.

    Both banks are set as basic RAM (no file) by default.

    When switching between banks, the memory bank should first be written (updated)
    then the bank being switched in can be read. Will need a public interface
    method for actual bank switching.


 **** NOTES ********************************************************************/