/* FileIO.cpp ***************************************
 *
 *   Acts to emulate a Raspberry PI Pico
 *		Implementing File Input / Output
 *		with the onboad flash ram and
 *		the SDCARD
 *
 * Copyright (C) 2024 by Jay Faries (GPL V3)
 ************************************/
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "C6809.hpp"
#include "Bus.hpp"
#include "FileIO.hpp"



Byte FileIO::read(Word offset, bool debug) 
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    switch (offset)
    {
        case FIO_ERR_FLAGS: 
        {
            data = fio_err_flags;  
            fio_err_flags = 0;
            break;
        }
        case FIO_PATH_LEN:  data = dir_data.size(); break;
        case FIO_PATH_POS:  
        {
            data = path_char_pos;
            if (data >= filePath.size())
                data = filePath.size() - 1;
            if (filePath.size() == 0)
                data = 0;
            break;
        }
        case FIO_PATH_DATA:
        {
            data = 0;
            if (filePath.size() > 0)
            {
                if (path_char_pos < filePath.size())
                {
                    data = (Byte)filePath.substr(path_char_pos, 1).at(0);
                    if (path_char_pos <= filePath.size())
                        path_char_pos++;
                }
                else
                {
                    filePath = "";
                    data = 0;
                }
            }
            break;
        }
        case FIO_DIR_DATA: // (Byte) a series of $0a--terminated filenames
        {
            data = 0;
            if (dir_data_pos < (int)dir_data.size())
            {
                data = (Byte)dir_data.substr(dir_data_pos, 1).at(0);
                if (dir_data_pos <= (int)dir_data.size())
                    dir_data_pos++;
            }
            else
            {
                dir_data = "";
                data = 0;
            }
            break;
        }
    }
    IDevice::write(offset,data);   // update any internal changes too
    return data;
}

void FileIO::write(Word offset, Byte data, bool debug) 
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    switch (offset)
    {
        case FIO_ERR_FLAGS: fio_err_flags = data; break;
        case FIO_PATH_POS:  
            if (data > filePath.size())
                data = filePath.size() - 1;
            path_char_pos = data;
            break;
        case FIO_PATH_DATA:
        {
            if (path_char_pos == 0)
            {
                filePath = "";
                path_char_pos = 0;

                filePath += data;
                path_char_pos++;
            }
            else
            {
                filePath = filePath.substr(0, path_char_pos);
                filePath += data;
                path_char_pos++;
            }
            break;
        }
        case FIO_COMMAND:
        {
            switch (data)
            {                
                case 0x00: _cmd_reset();                         break;
                case 0x01: _cmd_system_shutdown();               break;
                case 0x02: _cmd_system_load_comilation_date();   break;
                case 0x03: _cmd_new_file_stream();               break;
                case 0x04: _cmd_open_file();                     break;
                case 0x05: _cmd_is_file_open();                  break;
                case 0x06: _cmd_close_file();                    break;    
                case 0x07: _cmd_read_byte();                     break;    
                case 0x08: _cmd_write_byte();                    break;
                case 0x09: _cmd_load_hex_file();                 break;    
                case 0x0A: _cmd_get_file_length();               break;        
                case 0x0B: _cmd_list_directory();                break;    
                case 0x0C: _cmd_make_directory();                break;        
                case 0x0D: _cmd_change_directory();              break;
                case 0x0E: _cmd_get_current_path();              break;
                case 0x0F: _cmd_rename_directory();              break;
                case 0x10: _cmd_remove_directory();              break;    
                case 0x11: _cmd_delete_file();                   break;
                case 0x12: _cmd_rename_file();                   break;
                case 0x13: _cmd_copy_file();                     break;
                case 0x14: _cmd_seek_start();                    break;    
                case 0x15: _cmd_seek_end();                      break;
                case 0x16: _cmd_set_seek_position();             break;    
                case 0x17: _cmd_get_seek_position();             break;    
                default:
                    data = fio_err_flags | 0x02;    // invalid command
                    break;
            }
            break;
        }
    }

    IDevice::write(offset,data);   // update any internal changes too
}


void FileIO::_cmd_reset()
{
    // printf("FileIO: RESET Command Received\n");
    if (Bus::GetC6809())
        Bus::GetC6809()->reset();
}

void FileIO::_cmd_system_shutdown()
{
    Bus::IsRunning(false);
    // printf("FileIO: SHUTDOWN Command Received\n");
    // printf("\tShutting Down...\n");
}

void FileIO::_cmd_system_load_comilation_date()
{
    filePath = __DATE__;
    path_char_pos = 0;
    // printf("FileIO: COMPILE DATE Command Received\n");
    // printf("filePath: %s\n", filePath.c_str());
}

void FileIO::_cmd_new_file_stream()
{
}

void FileIO::_cmd_open_file()
{
}

void FileIO::_cmd_is_file_open()
{
}

void FileIO::_cmd_close_file()
{
}

void FileIO::_cmd_read_byte()
{
}

void FileIO::_cmd_write_byte()
{
}



// load_hex helpers
Byte FileIO::_fread_hex_byte(std::ifstream& ifs)
{
    char str[3];
    long l;
    ifs.get(str[0]);
    ifs.get(str[1]);
    str[2] = '\0';
    l = strtol(str, NULL, 16);
    return (Byte)(l & 0xff);
}
Word FileIO::_fread_hex_word(std::ifstream& ifs)
{
    Word ret;
    ret = _fread_hex_byte(ifs);
    ret <<= 8;
    ret |= _fread_hex_byte(ifs);
    return ret;
}

void FileIO::_cmd_load_hex_file()
{
    // // lambda to convert integer to hex string
    // auto hex = [](uint32_t n, uint8_t digits)
    // {
    // 	std::string s(digits, '0');
    // 	for (int i = digits - 1; i >= 0; i--, n >>= 4)
    // 		s[i] = "0123456789ABCDEF"[n & 0xF];
    // 	return s;
    // };

    //printf("Change Directory To: %s\n", filePath.c_str());
    if (filePath.size() == 0)   return;

     // remove any trailing spaces
     while (filePath.substr(filePath.size()-2,1) == " ")
         filePath = filePath.substr(0, filePath.size()-2);

    std::ifstream ifs(filePath);
    std::filesystem::path f{ filePath.c_str()};
    if (!std::filesystem::exists(f))
    {
        printf("File '%s' Not Found\n", f.filename().string().c_str());
        Byte errData = Bus::Read(FIO_ERR_FLAGS);
        errData |= 0x80;     // file was not found
        Bus::Write(FIO_ERR_FLAGS, errData);
        ifs.close();
        return;
    }
    // does this have a .hex extenion?
    // (wrong file type)
    std::string strExt = f.filename().extension().string();
    if (strExt != ".hex" && strExt != ".hex ")
    {
        printf("EXTENSION: %s\n", strExt.c_str());

        Byte errData = Bus::Read(FIO_ERR_FLAGS);
        errData |= 0x04;     // (wrong file type)
        Bus::Write(FIO_ERR_FLAGS, errData);
        ifs.close();
        return;
    }
 
    if (!ifs.is_open())
    {
        printf("UNABLE TO OPEN FILE '%s'\n", f.filename().string().c_str());
        Byte errData = Bus::Read(FIO_ERR_FLAGS);
        errData |= 0x20;     // file not open
        Bus::Write(FIO_ERR_FLAGS, errData);
        ifs.close();
        return;
    }
    bool done = false;
    char c;
    while (!done)
    {
        Byte n, t;
        Word addr;
        Byte b;
        ifs.get(c);	// test the leading ":"
        if (c != ':')
        {
            //printf(": not found\n");
            ifs.close();
            Byte errData = Bus::Read(FIO_ERR_FLAGS);
            errData |= 0x04;     // (wrong file type)
            Bus::Write(FIO_ERR_FLAGS, errData);
            return;
        }
        n = _fread_hex_byte(ifs);		// byte count for this line
        addr = _fread_hex_word(ifs);	// fetch the begin address		
        t = _fread_hex_byte(ifs);		// record type
        if (t == 0x00)
        {
            while (n--)
            {
                b = _fread_hex_byte(ifs);
                // std::cout << "0x" << hex(addr,4) << ":";
                // std::cout << "0x" << hex(b, 2) << std::endl;
                Bus::Write(addr, b, true);
                ++addr;
            }
            // Read and discard checksum byte
            (void)_fread_hex_byte(ifs);
            // skip the junk at the end of the line	
            if (ifs.peek() == '\r')	ifs.get(c);
            if (ifs.peek() == '\n')	ifs.get(c);
        }
        else if (t == 0x01)
            done = true;
    }
    // close and return
    ifs.close();
}

void FileIO::_cmd_get_file_length()
{
}


void FileIO::_cmd_list_directory()
{
    std::string current_path = std::filesystem::current_path().generic_string();
    std::vector<std::string> _files;

    // initial error checking
    if (current_path[0] == -1)   current_path = "";
    if (filePath[0] == -1)    
    {
        filePath="";
        path_char_pos = 0;
    }
    std::string seach_folder = "";
    std::filesystem::path arg1 = filePath;    
    if (arg1=="") arg1=std::filesystem::current_path().generic_string();
    std::string filename = arg1.filename().generic_string().c_str();
    std::string stem = arg1.stem().generic_string().c_str();
    std::string extension = arg1.extension().generic_string().c_str();
    std::string app_str = current_path + "/" + arg1.generic_string().c_str();

    if (std::filesystem::is_directory(app_str))
        arg1 = app_str; //printf("IS DIRECTORY\n");

    try
    {    
        // is folder
        if (std::filesystem::is_directory(arg1))
        {
            // dir_data += arg1;       
            // dir_data += "=->\n";  
            seach_folder = arg1.generic_string().c_str();          
            for (const auto& entry : std::filesystem::directory_iterator(arg1.generic_string().c_str()))
            {
                std::stringstream ssFile;
                std::string entry_filename = entry.path().filename().generic_string().c_str();
                if (entry.is_directory())
                {
                    ssFile << "  [" << entry_filename << "]";
                    _files.push_back(ssFile.str());
                }
                else
                {
                    ssFile << "    " << entry_filename;
                    _files.push_back(ssFile.str());
                }                
            }
        }
        // is file and/or wildcards
        else
        {            
            std::string parent = std::filesystem::path(arg1).parent_path().generic_string().c_str();
            app_str = current_path + "/" + parent;  //arg1.generic_string().c_str();
            seach_folder = app_str.c_str();      
            for (const auto& entry : std::filesystem::directory_iterator(app_str.c_str()))
            {
                if (entry.is_regular_file())
                {
                    std::stringstream ssFile;
                    std::string file_stem = entry.path().stem().generic_string().c_str();
                    std::string file_ext = entry.path().extension().generic_string().c_str();
                    std::string entry_filename = entry.path().filename().generic_string().c_str();

                    bool bAllWild = false;
                    if (stem == "*" && extension == ".*")
                        bAllWild = true;
                        
                    if (entry_filename == arg1.filename().generic_string().c_str() || bAllWild)
                    {
                        ssFile << "    " << entry_filename;
                        _files.push_back(ssFile.str());
                    }
                    else if (file_stem == stem && extension == ".*")
                    {
                        ssFile << "    " << entry_filename;
                        _files.push_back(ssFile.str());
                    }                    
                    else if (file_ext == extension && stem == "*")
                    {
                        ssFile << "    " << entry_filename;
                        _files.push_back(ssFile.str());
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        // printf("EXCEPTION: %s\n",e.what());
        dir_data = "Error: No such folder!\n";
        dir_data_pos = 0; 
        return;
    }
    // clear the FIO_BUFFER
    for (int i = FIO_BUFFER; i <= FIO_BFR_END; i++)
        Bus::Write(i, 0);

    // build the result
    dir_data = seach_folder;
    dir_data += "\n";
    std::sort(_files.begin(), _files.end());
    for (auto& f : _files)
        dir_data += (f+"\n");
    dir_data_pos = 0;      
}

void FileIO::_cmd_make_directory()
{
}

void FileIO::_cmd_change_directory()
{
    printf("Change Directory To: %s\n", filePath.c_str());
    if (filePath.size() == 0)   return;

    // // remove any trailing spaces
    // while (filePath.substr(filePath.size() - 2, 1) == " ")
    //     filePath = filePath.substr(0, filePath.size() - 2);

    std::string chdir = filePath;
    
    if (std::filesystem::exists(chdir))
    {
        printf("Directory Found\n");
        Byte data = Bus::Read(FIO_ERR_FLAGS);
        data &= ~0x40;
        Bus::Write(FIO_ERR_FLAGS, data);
        fio_err_flags = data;
        std::filesystem::current_path(chdir);
    }
    else
    {
        printf("ERROR: Directory Not Found!\n");
        Byte data = Bus::Read(FIO_ERR_FLAGS);
        data |= 0x40;
        Bus::Write(FIO_ERR_FLAGS, data);
        fio_err_flags = data;
    }
}

void FileIO::_cmd_get_current_path()
{
    // printf("FileIO::_cmd_get_current_path()\n");
    path_char_pos = 0;
    filePath = std::filesystem::current_path().string() + "\n";
    printf("%s\n", filePath.c_str());
}

void FileIO::_cmd_rename_directory()
{
}

void FileIO::_cmd_remove_directory()
{
}

void FileIO::_cmd_delete_file()
{
}

void FileIO::_cmd_rename_file()
{
}

void FileIO::_cmd_copy_file()
{
}

void FileIO::_cmd_seek_start()
{
}

void FileIO::_cmd_seek_end()
{
}

void FileIO::_cmd_set_seek_position()
{
}

void FileIO::_cmd_get_seek_position()
{
}

Word FileIO::OnAttach(Word nextAddr) 
{
    // printf("%s::OnAttach()\n", Name().c_str());  

    Word old_addr = nextAddr; 

	DisplayEnum("FIO_BEGIN", nextAddr, "Start of the FileIO register space");
	nextAddr += 0;

    //DisplayEnum("", 0, "");
    DisplayEnum("FIO_ERR_FLAGS", nextAddr, "(Byte) File IO error flags");
    DisplayEnum("", 0, "FIO_ERR_FLAGS: ABCD.EFGH");
    DisplayEnum("", 0, "     A:  file was not found");
    DisplayEnum("", 0, "     B:  directory was not found");
    DisplayEnum("", 0, "     C:  file not open");
    DisplayEnum("", 0, "     D:  end of file");
    DisplayEnum("", 0, "     E:  buffer overrun");
    DisplayEnum("", 0, "     F:  wrong file type");
    DisplayEnum("", 0, "     G:  invalid command");
    DisplayEnum("", 0, "     H:  incorrect file stream");
    nextAddr += 1;

    DisplayEnum("", 0, "");
    DisplayEnum("FIO_COMMAND", nextAddr, "(Byte) OnWrite, execute a file command (FC_<cmd>)");
    nextAddr += 1;
    Byte enumID = 0;
    DisplayEnum("", 0, "Begin FIO_COMMANDS");
    DisplayEnum("FC_RESET",     enumID++, "       Reset");
    DisplayEnum("FC_SHUTDOWN",  enumID++, "       SYSTEM: Shutdown");
    DisplayEnum("FC_COMPDATE",  enumID++, "       SYSTEM: Load Compilation Date");
    DisplayEnum("FC_NEWFILE",   enumID++, "     * New File Stream");
    DisplayEnum("FC_OPENFILE",  enumID++, "     * Open File");
    DisplayEnum("FC_ISOPEN",    enumID++, "     *Is File Open ? (returns FIO_ERR_FLAGS bit - 5)");
    DisplayEnum("FC_CLOSEFILE", enumID++, "     * Close File");
    DisplayEnum("FC_READBYTE",  enumID++, "     * Read Byte (into FIO_IOBYTE)");
    DisplayEnum("FC_WRITEBYTE", enumID++, "     * Write Byte (from FIO_IOBYTE)");
    DisplayEnum("FC_LOADHEX",   enumID++, "     * Load Hex Format File");
    DisplayEnum("FC_GETLENGTH", enumID++, "     * Get File Length (into FIO_IOWORD)");
    DisplayEnum("FC_LISTDIR",   enumID++, "       List Directory");
    DisplayEnum("FC_MAKEDIR",   enumID++, "     * Make Directory");
    DisplayEnum("FC_CHANGEDIR", enumID++, "       Change Directory");
    DisplayEnum("FC_GETPATH",   enumID++, "       Fetch Current Path");
    DisplayEnum("FC_REN_DIR",   enumID++, "     * Rename Directory");
    DisplayEnum("FC_DEL_DIR",   enumID++, "     * Delete Directory");
    DisplayEnum("FC_DEL_FILE",  enumID++, "     * Delete File");
    DisplayEnum("FC_REN_FILE",  enumID++, "     * Rename file");
    DisplayEnum("FC_COPYFILE",  enumID++, "     * Copy File");
    DisplayEnum("FC_SEEKSTART", enumID++, "     * Seek Start");
    DisplayEnum("FC_SEEKEND",   enumID++, "     * Seek End");
    DisplayEnum("FC_SET_SEEK",  enumID++, "     * Set Seek Position (from FIO_IOWORD)");
    DisplayEnum("FC_GET_SEEK",  enumID++, "     * Get Seek Position (into FIO_IOWORD)");
    DisplayEnum("", 0, "End FIO_COMMANDS");
    DisplayEnum("", 0, "");

    DisplayEnum("FIO_STREAM", nextAddr, "(Byte) current file stream index (0-15)");
    nextAddr += 1;

    DisplayEnum("FIO_MODE", nextAddr, "(Byte) Flags describing the I/O mode for the file");
    DisplayEnum("", 0, "FIO_MODE: 00AB.CDEF  (indexed by FIO_STREAM)");
    DisplayEnum("", 0, "     A:  INPUT - File open for reading");
    DisplayEnum("", 0, "     B:  OUTPUT - File open for writing");
    DisplayEnum("", 0, "     C:  BINARY - 1: Binary Mode, 0: Text Mode");
    DisplayEnum("", 0, "     D:  AT_END - Output starts at the end of the file");
    DisplayEnum("", 0, "     E:  APPEND - All output happens at end of the file");
    DisplayEnum("", 0, "     F:  TRUNC - discard all previous file data");
    nextAddr += 1;

    DisplayEnum("FIO_SEEKPOS", nextAddr, "(DWord) file seek position");
    nextAddr += 4;

    DisplayEnum("FIO_IOBYTE", nextAddr, "(Byte) input / output character");
    nextAddr += 1;

    DisplayEnum("FIO_IOWORD", nextAddr, "(Byte) input / output character");
    nextAddr += 1;

    DisplayEnum("FIO_PATH_LEN", nextAddr, "(Byte) length of the filepath");
    nextAddr += 1;

    DisplayEnum("FIO_PATH_POS", nextAddr, "(Byte) character position within the filepath");
    nextAddr += 1;

    DisplayEnum("FIO_PATH_DATA", nextAddr, "(Byte) data at the character position of the path");
    nextAddr += 1;

    DisplayEnum("FIO_DIR_DATA", nextAddr, "(Byte) a series of null-terminated filenames");
    DisplayEnum("", 0, "    NOTES: Current read-position is reset to the beginning following a ");
    DisplayEnum("", 0, "            List Directory command. The read-position is automatically ");
    DisplayEnum("", 0, "            advanced on read from this register. Each filename is ");
    DisplayEnum("", 0, "            $0a-terminated. The list itself is null-terminated.");
    nextAddr += 1;

	DisplayEnum("FIO_END", nextAddr, "End of the FileIO register space");
	nextAddr += 0;

    return nextAddr - old_addr;

}
void FileIO::OnInit() 
{
    // printf("%s::OnInit()\n", Name().c_str());    

    // create a block of null file stream devices
    for (int i = 0; i < _FSTREAM_MAX; i++)
    {
        std::fstream* ifs = nullptr;
        _vecFileStreams.push_back(ifs);
    }
}

void FileIO::OnQuit() 
{
    // printf("%s::OnQuit()\n", Name().c_str());    

    // close all open file streams
    for (auto* fs : _vecFileStreams)
    {
        if (fs != nullptr)
        {
            if (fs->is_open())
            {
                fs->close();
                fs = nullptr;
            }
        }
    }
}

