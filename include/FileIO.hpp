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

    private:
        // File IO System Commands
        void _cmd_reset();
        void _cmd_system_shutdown();
        void _cmd_system_load_comilation_date();
        void _cmd_new_file_stream();
        void _cmd_open_file();
        void _cmd_is_file_open();
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

        //enum FMODE
        //{
        //    NONE,
        //    INPUT,
        //    OUTPUT,
        //    BINARY,
        //    AT_END,
        //    APPEND,
        //    TRUNC,
        //};
        //struct FILE_NODE
        //{
        //    std::fstream* ptrStream = nullptr;
        //    FMODE fmode = NONE;
        //    std::string path = "";
        //};

        const int _FSTREAM_MAX = 16;    // max number of file streams
        Byte _fileStreamIndex = 0;      // file stream index
        std::vector<std::fstream*> _vecFileStreams; // vector of file streams

        Byte fio_err_flags = 0;     // error flags

        Byte path_char_pos = 0;   // active character position within the file path string
        std::string filePath = "";  // the current file path

        int dir_data_pos = 0;     // position within the dir_data string
        std::string dir_data;       // directory filename list container
};
