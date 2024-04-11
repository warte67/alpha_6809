/* FileIO.hpp ***************************************
 *
 *   Acts to emulate a Raspberry PI Pico
 *		Implementing File Input / Output
 *		with the onboad flash ram and
 *		the SDCARD
 *
 * Copyright (C) 2024 by Jay Faries (GPL V3)
 ************************************/
#pragma once

#include "IDevice.hpp" 

class FileIO : public IDevice
{
    public:
        FileIO() { _deviceName = "FileIO"; }
        FileIO(std::string sName) : IDevice(sName) {}
        ~FileIO() {};

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;

        // unused pure virtuals
		void OnActivate() override {};
		void OnDeactivate() override {};
		void OnEvent(SDL_Event* evnt) override {};
		void OnUpdate(float fElapsedTime) override {};
		void OnRender() override {};

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        enum FILE_ERROR {
            FE_NOERROR=0,   //  $00: no error, condition normal
            FE_NOTFOUND,    //  $01: file or folder not found
            FE_NOTOPEN,     //  $02: file not open
            FE_EOF,         //  $03: end of file
            FE_OVERRUN,     //  $04: buffer overrun
            FE_WRONGTYPE,   //  $05: wrong file type
            FE_BAD_CMD,     //  $06: invalid command
            FE_BADSTREAM    //  $07: invalid file stream
        };

    private:
        // File IO System Commands
        void _cmd_reset();
        void _cmd_system_shutdown();
        void _cmd_system_load_comilation_date();
        bool _cmd_does_file_exist();
        void _cmd_open_read();
        void _cmd_open_write();
        void _cmd_open_append();
        void _cmd_close_file();
        void _cmd_read_byte();
        void _cmd_write_byte();
        void _cmd_load_hex_file();
        void _cmd_get_file_length();
        void _cmd_list_directory();
        void _cmd_make_directory();
        void _cmd_change_directory();
        void _cmd_get_current_path();
        void _cmd_rename_directory();
        void _cmd_remove_directory();
        void _cmd_delete_file();
        void _cmd_rename_file();
        void _cmd_copy_file();
        void _cmd_seek_start();
        void _cmd_seek_end();
        void _cmd_set_seek_position();
        void _cmd_get_seek_position();
        Byte _fread_hex_byte(std::ifstream& ifs);
        Word _fread_hex_word(std::ifstream& ifs);


        int _FindOpenFileSlot();    // return a handle to an open file stream slot
        bool _bFileExists(const char* file);    // checks to see if a file exists
        void _openFile(const char* mode);       // file open helper

        Byte _fileHandle = 0;      // file stream index (HANDLE)
        std::vector<FILE*> _vecFileStreams; // vector of file streams

        // Byte fio_err_flags = 0;     // error flags (deprecated)
        FILE_ERROR fio_error_code = FILE_ERROR::FE_NOERROR;    // current error code

        Byte path_char_pos = 0;   // active character position within the file path string
        std::string filePath = "";  // the current file path

        int dir_data_pos = 0;     // position within the dir_data string
        std::string dir_data;       // directory filename list container

        Byte  _io_data = 0;     // data to read / write
        DWord _seek_pos = 0;    // file seek position
};
