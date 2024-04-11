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
#include <cstdio>
#include <cstdlib>
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
        case FIO_ERROR: 
        {
            data = fio_error_code;
            fio_error_code = FILE_ERROR::FE_NOERROR;
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
        case FIO_COMMAND:   break;
        case FIO_HANDLE:    data = _fileHandle; break;
        case FIO_SEEKPOS+0:   break;
        case FIO_SEEKPOS+1:   break;
        case FIO_SEEKPOS+2:   break;
        case FIO_SEEKPOS+3:   break;
        case FIO_IODATA:    data = _io_data; break;
    }
    IDevice::write(offset,data);   // update any internal changes too
    return data;
}

void FileIO::write(Word offset, Byte data, bool debug) 
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    switch (offset)
    {
        case FIO_ERROR: fio_error_code = (FILE_ERROR)data; break;
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
                case FC_RESET:      _cmd_reset();                         break;
                case FC_SHUTDOWN:   _cmd_system_shutdown();               break;
                case FC_COMPDATE:   _cmd_system_load_comilation_date();   break;
                case FC_FILEEXISTS: _cmd_does_file_exist();               break;
                case FC_OPENREAD:   _cmd_open_read();                     break;
                case FC_OPENWRITE:  _cmd_open_write();                    break;
                case FC_OPENAPPEND: _cmd_open_append();                   break;
                case FC_CLOSEFILE:  _cmd_close_file();                    break;    
                case FC_READBYTE:   _cmd_read_byte();                     break;    
                case FC_WRITEBYTE:  _cmd_write_byte();                    break;
                case FC_LOADHEX:    _cmd_load_hex_file();                 break;    
                case FC_GETLENGTH:  _cmd_get_file_length();               break;        
                case FC_LISTDIR:    _cmd_list_directory();                break;    
                case FC_MAKEDIR:    _cmd_make_directory();                break;        
                case FC_CHANGEDIR:  _cmd_change_directory();              break;
                case FC_GETPATH:    _cmd_get_current_path();              break;
                case FC_REN_DIR:    _cmd_rename_directory();              break;
                case FC_DEL_DIR:    _cmd_remove_directory();              break;    
                case FC_DEL_FILE:   _cmd_delete_file();                   break;
                case FC_REN_FILE:   _cmd_rename_file();                   break;
                case FC_COPYFILE:   _cmd_copy_file();                     break;
                case FC_SEEKSTART:  _cmd_seek_start();                    break;    
                case FC_SEEKEND:    _cmd_seek_end();                      break;
                case FC_SET_SEEK:   _cmd_set_seek_position();             break;    
                case FC_GET_SEEK:   _cmd_get_seek_position();             break;    
                default:
                    data = fio_error_code = FILE_ERROR::FE_BAD_CMD;    // invalid command
                    break;
            }
            break;
        }
        case FIO_HANDLE:    _fileHandle = data; break;
        case FIO_SEEKPOS+0:   break;
        case FIO_SEEKPOS+1:   break;
        case FIO_SEEKPOS+2:   break;
        case FIO_SEEKPOS+3:   break;
        case FIO_IODATA:    _io_data = data; break;
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
}

void FileIO::_cmd_system_load_comilation_date()
{
    filePath = __DATE__;
    path_char_pos = 0;
}

bool FileIO::_bFileExists(const char* file)
{
    if (file)
    {
        FILE* fp = fopen(file, "rb");
        if (!fp)
        {
            Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTFOUND);
            return false;
        }
        fclose(fp);
        return true;            
    }    
    return false;
}

void FileIO::_openFile(const char* mode)
{
    // if (!_bFileExists(filePath.c_str())) { return; }
	_fileHandle = _FindOpenFileSlot();
    _vecFileStreams[_fileHandle] = fopen(filePath.c_str(), mode);
    if (!_vecFileStreams[_fileHandle])
    {
        Bus::Write(FIO_ERROR, FE_NOTOPEN);
        return;
    } 
}

void FileIO::_cmd_open_read()
{
    printf("%s::_cmd_open_read()\n", Name().c_str());
    _openFile("rb");
}

void FileIO::_cmd_open_write()
{
    printf("%s::_cmd_open_write()\n", Name().c_str());
    _openFile("wb");
}
void FileIO::_cmd_open_append()
{
    printf("%s::_cmd_open_append()\n", Name().c_str());
    _openFile("ab");
}

void FileIO::_cmd_close_file()
{
    printf("%s::_cmd_close_file()\n", Name().c_str());
    Byte handle = Bus::Read(FIO_HANDLE);
    if (handle==0 || _vecFileStreams[handle] == nullptr)
    {
        Bus::Write(FIO_ERROR, FE_NOTOPEN);
        return;
    }
    fclose(_vecFileStreams[handle]);
    _vecFileStreams[handle] = nullptr;
}

void FileIO::_cmd_read_byte()
{
    // printf("%s::_cmd_read_byte()\n", Name().c_str());
    Byte handle = Bus::Read(FIO_HANDLE);
    if (handle == 0 || _vecFileStreams[handle] == nullptr)
    {   
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTOPEN);
        return;
    }
    Byte data = (Byte)fgetc(_vecFileStreams[handle]);
    if (feof(_vecFileStreams[handle]))
    {
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_OVERRUN);
        // _cmd_close_file();
        return;
    }
    Bus::Write(FIO_IODATA, data);    
}

void FileIO::_cmd_write_byte()
{
    // printf("%s::_cmd_write_byte()\n", Name().c_str());
    Byte handle = Bus::Read(FIO_HANDLE);
    if (handle == 0 || _vecFileStreams[handle] == nullptr)
    {
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTOPEN);
        return;
    }
    Byte data = (Byte)fputc(_io_data, _vecFileStreams[handle]);
    Bus::Write(FIO_IODATA, data);
}

bool FileIO::_cmd_does_file_exist()
{
    bool exists = _bFileExists(filePath.c_str());
    Bus::Write(FIO_IODATA, (Byte)exists);
    if (!exists)
    {
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTFOUND);
        return false;
    }
    return true;
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

    // sanity check (insanity perhaps?)
    if (filePath.size() == 0)   return;

    if (filePath[0]=='\"')
        filePath = filePath.substr(1, filePath.size()-3);

    // printf("'%s' size: %d\n",filePath.c_str(), (int)filePath.size());   
    path_char_pos=0;


    std::ifstream ifs(filePath);
    std::filesystem::path f{ filePath.c_str()};
    if (!std::filesystem::exists(f))
    {
        // printf("File '%s' Not Found\n", f.filename().string().c_str());
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTFOUND);
        ifs.close();
        return;
    }
    // does this have a .hex extenion?
    // (wrong file type)
    std::string strExt = f.filename().extension().string();
    if (strExt != ".hex" && strExt != ".hex ")
    {
        // printf("EXTENSION: %s\n", strExt.c_str());
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_WRONGTYPE);
        ifs.close();
        return;
    }
 
    if (!ifs.is_open())
    {
        // printf("UNABLE TO OPEN FILE '%s'\n", f.filename().string().c_str());
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTOPEN);
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
        ifs.get(c);	
        if (c != ':')
        {
            ifs.close();
            Bus::Write(FIO_ERROR, FILE_ERROR::FE_WRONGTYPE);
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
    // printf("%s::_cmd_get_file_length()\n", Name().c_str());
    std::filesystem::path arg1 = filePath;
    if (!std::filesystem::exists(arg1))
    {
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTFOUND);  
        Bus::Write(MATH_ACR_INT, 0);
        return;
    }
    Uint32 file_size = std::filesystem::file_size(arg1);
    Bus::Write_DWord(MATH_ACR_INT, file_size);
}


void FileIO::_cmd_list_directory()
{
    printf("%s::_cmd_list_directory()\n", Name().c_str());

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
    std::string app_str = arg1.generic_string().c_str();   
    if (arg1=="") 
    {
        arg1=std::filesystem::current_path().generic_string();
        app_str = current_path;   
    }
    else
        app_str = current_path + "/" + arg1.generic_string().c_str();   
    std::string filename = arg1.filename().generic_string().c_str();
    std::string stem = arg1.stem().generic_string().c_str();
    std::string extension = arg1.extension().generic_string().c_str();


    if (std::filesystem::is_directory(app_str))
        arg1 = app_str;

    try
    {    
        // is folder
        if (std::filesystem::is_directory(arg1))
        {
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
            app_str = current_path + "/" + parent;
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

void FileIO::_cmd_change_directory()
{
    // printf("Change Directory To: %s\n", filePath.c_str());
    if (filePath.size() == 0)   return;

    std::string chdir = filePath;    
    if (std::filesystem::exists(chdir))
        std::filesystem::current_path(chdir);
    else
        Bus::Write(FIO_ERROR, FILE_ERROR::FE_NOTFOUND);
}

void FileIO::_cmd_get_current_path()
{
    // printf("FileIO::_cmd_get_current_path()\n");
    path_char_pos = 0;
    filePath = std::filesystem::current_path().string() + "\n";
    printf("%s\n", filePath.c_str());
}

void FileIO::_cmd_make_directory()
{
    printf("FileIO::_cmd_make_directory()\n");
}

void FileIO::_cmd_rename_directory()
{
    printf("FileIO::_cmd_rename_directory()\n");
}

void FileIO::_cmd_remove_directory()
{
    printf("FileIO::_cmd_remove_directory()\n");
}

void FileIO::_cmd_delete_file()
{
    printf("FileIO::_cmd_delete_file()\n");
}

void FileIO::_cmd_rename_file()
{
    printf("FileIO::_cmd_rename_file()\n");
}

void FileIO::_cmd_copy_file()
{
    printf("FileIO::_cmd_copy_file()\n");
}

void FileIO::_cmd_seek_start()
{
    printf("FileIO::_cmd_seek_start()\n");
}

void FileIO::_cmd_seek_end()
{
    printf("FileIO::_cmd_seek_end()\n");
}

void FileIO::_cmd_set_seek_position()
{
    printf("FileIO::_cmd_set_seek_position()\n");
}

void FileIO::_cmd_get_seek_position()
{
    printf("FileIO::_cmd_get_seek_position()\n");
}


// helper: return a handle to an open file stream slot
int FileIO::_FindOpenFileSlot()
{
	// find an empty slot
	int found = 0;
	for (int t = 1; t < FILEHANDLESMAX; t++)
	{
		if (_vecFileStreams[t] == nullptr)
		{
			found = t;
			break;
		}
	}
	// too many file handles?
	if (found == 0)
	{
		Bus::Write(FIO_ERROR, FILE_ERROR::FE_BADSTREAM);
		return 0;
	}
	Bus::Write(FIO_HANDLE, found);
	return found;
}


Word FileIO::OnAttach(Word nextAddr) 
{
    // printf("%s::OnAttach()\n", Name().c_str());  

    Word old_addr = nextAddr; 

	DisplayEnum("FIO_BEGIN", nextAddr, "Start of the FileIO register space");
	nextAddr += 0;

    Byte er_idx = 0;
    DisplayEnum("FIO_ERROR",    nextAddr, "(Byte) FILE_ERROR enumeration result");
    DisplayEnum("", 0, "Begin FILE_ERROR enumeration");
    DisplayEnum("FE_NOERROR",   er_idx++, "     $00: no error, condition normal");
    DisplayEnum("FE_NOTFOUND",  er_idx++, "     $01: file or folder not found");
    DisplayEnum("FE_NOTOPEN",   er_idx++, "     $02: file not open");
    DisplayEnum("FE_EOF",       er_idx++, "     $03: end of file");
    DisplayEnum("FE_OVERRUN",   er_idx++, "     $04: buffer overrun");
    DisplayEnum("FE_WRONGTYPE", er_idx++, "     $05: wrong file type");
    DisplayEnum("FE_BAD_CMD",   er_idx++, "     $06: invalid command");
    DisplayEnum("FE_BADSTREAM", er_idx++, "     $07: invalid file stream");
    DisplayEnum("", 0, "End FILE_ERROR enumeration");
    nextAddr += 1;

    DisplayEnum("", 0, "");
    DisplayEnum("FIO_COMMAND", nextAddr, "(Byte) OnWrite, execute a file command (FC_<cmd>)");
    nextAddr += 1;
    Byte enumID = 0;
    DisplayEnum("", 0, "Begin FIO_COMMANDS");
    DisplayEnum("FC_RESET",     enumID++, "       Reset");
    DisplayEnum("FC_SHUTDOWN",  enumID++, "       SYSTEM: Shutdown");
    DisplayEnum("FC_COMPDATE",  enumID++, "       SYSTEM: Load Compilation Date");
    DisplayEnum("FC_FILEEXISTS",enumID++, "       Does File Exist (return in FIO_IODATA)");
    DisplayEnum("FC_OPENREAD",  enumID++, "     * Open Binary File For Reading");
    DisplayEnum("FC_OPENWRITE", enumID++, "     * Open Binary File For Writing");
    DisplayEnum("FC_OPENAPPEND",enumID++, "     * Open Binary File For Appending");
    DisplayEnum("FC_CLOSEFILE", enumID++, "     * Close File");
    DisplayEnum("FC_READBYTE",  enumID++, "     * Read Byte (into FIO_IOBYTE)");
    DisplayEnum("FC_WRITEBYTE", enumID++, "     * Write Byte (from FIO_IOBYTE)");
    DisplayEnum("FC_LOADHEX",   enumID++, "       Load Hex Format File");
    DisplayEnum("FC_GETLENGTH", enumID++, "       Get File Length (into FIO_IOWORD)");
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

    DisplayEnum("FIO_HANDLE", nextAddr, "(Byte) current file stream HANDLE 0=NONE");
    nextAddr += 1;

    DisplayEnum("FIO_SEEKPOS", nextAddr, "(DWord) file seek position");
    nextAddr += 4;

    DisplayEnum("FIO_IODATA", nextAddr, "(Byte) input / output character");
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
    for (int i = 0; i < FILEHANDLESMAX; i++)
    {
        FILE* ifs = nullptr;
        _vecFileStreams.push_back(ifs);
    }
}

void FileIO::OnQuit() 
{
    // printf("%s::OnQuit()\n", Name().c_str());    

    // close and and all open file streams
    for (auto* fs : _vecFileStreams)
    {
        if (fs != nullptr)
        {
            fclose(fs);
            fs = nullptr;
        }
    }
}

