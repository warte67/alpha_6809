// *************************************************
// *
// * MemBank.cpp
// *
// ***********************************

#include <sstream>
#include "Bus.hpp"
#include "MemBank.hpp"

Byte MemBank::read(Word offset, bool debug) 
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    // BANK ONE = $B000-$CFFF
    // BANK TWO = $D000-$EFFF    

    // ...

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}
void MemBank::write(Word offset, Byte data, bool debug) 
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    // BANK ONE = $B000-$CFFF
    // BANK TWO = $D000-$EFFF    

    // DO NOT allow writes when ROM is specified in this bank
    // ...

    IDevice::write(offset,data);   // update any internal changes too
}

Word MemBank::OnAttach(Word nextAddr) 
{
    // printf("%s::OnAttach()\n", Name().c_str());    

    Word old_addr = nextAddr;

    DisplayEnum("", 0, "");
    DisplayEnum("", 0, "Banked Memory Buffers (16K total):");
    DisplayEnum("MEMBANK_BEGIN", nextAddr, " start of Banked Memory Region");

    DisplayEnum("MEMBANK_ONE", nextAddr, " Start of Page One (8K)");
    nextAddr += 8*1024;

    DisplayEnum("MEMBANK_TWO", nextAddr, " Start of Page Two (8K)");
    nextAddr += 8*1024;

    DisplayEnum("MEMBANK_END", nextAddr, " End of Banked Memory Region");

    return nextAddr - old_addr;    return 0;
}
void MemBank::OnInit() 
{
    // printf("%s::OnInit()\n", Name().c_str());   

    // create a new default 'paged.mem' file if one does not yet exist
    _newDefaultFile();

    // load the 'paged.mem' header info
    _loadHeader();

    // load the current banked memory pages from the 'paged.mem' file
    // ...

    for (int t=0; t<256; t++)
    {
        printf("$%04X\n", _bank_header.bank_node[t].seek_pos);
    }
}
void MemBank::OnQuit() 
{
    // printf("%s::OnQuit()\n", Name().c_str());    

    // save the 'paged.mem' header info
    _saveHeader();

    // save the current banked memory pages to the 'paged.mem' file
    // ...
}

// create a new 'paged.mem' bank file if one does not already exist
// returns false on failure
bool MemBank::_newDefaultFile()
{
    if (!_fileExists(PAGED_MEMORY_FILENAME))
    {
        FILE* fp = fopen(PAGED_MEMORY_FILENAME.c_str(), "wb");
        if (fp==nullptr)        
        {
            std::stringstream ss;
            ss << "Error creating the paged memory file: \n" << PAGED_MEMORY_FILENAME << std::endl;
            Bus::Error(ss.str());
            return false;
        }
        // write the blank default header
        DWord seek_pos = sizeof(BANK_HEADER);
        for (int t=0; t<256; t++) {
            _bank_header.bank_node[t].seek_pos = seek_pos;
            seek_pos += PAGED_MEMORY_BANKSIZE;
        }
        fwrite((void *)&_bank_header, sizeof(Byte), sizeof(_bank_header), fp);
        // write the paged data (2 Mbytes)
        DWord buffer_size = 256*PAGED_MEMORY_BANKSIZE;
        for (DWord t=0; t<buffer_size; t++)
            fputc(0,fp);    // initially all zeroes
        fclose(fp);
    }    
    return true;
}

// returns true if the file exists
bool MemBank::_fileExists(const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "rb");
    if (fp==nullptr)
        return false;
    fclose(fp);
    return true;
}

// load the header info from the 'paged.mem' file
bool MemBank::_loadHeader()
{
    if (!_fileExists(PAGED_MEMORY_FILENAME)) {
        std::stringstream ss;
        ss << "Unable to find the paged memory file: \n" << PAGED_MEMORY_FILENAME << std::endl;
        Bus::Error(ss.str());
        return false;
    }
    FILE* fp = fopen(PAGED_MEMORY_FILENAME.c_str(), "rb+");
    if (!fp) {
        std::stringstream ss;
        ss << "Unable to open the paged memory file: \n" << PAGED_MEMORY_FILENAME << std::endl;
        Bus::Error(ss.str());
        return false;
    }
    fseek(fp, 0, SEEK_SET);     // seek the beginning of the file
    fread((void *)&_bank_header, sizeof(Byte), sizeof(BANK_HEADER), fp);
    fclose(fp);
    return true;
}

// save the header info to the 'paged.mem' file
bool MemBank::_saveHeader()
{
    if (!_fileExists(PAGED_MEMORY_FILENAME)) {
        std::stringstream ss;
        ss << "Unable to find the paged memory file: \n" << PAGED_MEMORY_FILENAME << std::endl;
        Bus::Error(ss.str());
        return false;
    }
    FILE* fp = fopen(PAGED_MEMORY_FILENAME.c_str(), "rb+");
    if (!fp) {
        std::stringstream ss;
        ss << "Unable to open the paged memory file: \n" << PAGED_MEMORY_FILENAME << std::endl;
        Bus::Error(ss.str());
        return false;
    }
    fseek(fp, 0, SEEK_SET);     // seek the beginning of the file
    fwrite((void *)&_bank_header, sizeof(Byte), sizeof(BANK_HEADER), fp);
    fclose(fp);
    return true;
}


// void MemBank::OnActivate() 
// {
//     // printf("%s::OnActivate()\n", Name().c_str());    
// }
// void MemBank::OnDeactivate() 
// {
//     // printf("%s::OnDeactivate()\n", Name().c_str());    
// }
// void MemBank::OnEvent(SDL_Event* evnt) 
// {
//     // printf("%s::OnEvent()\n", Name().c_str());    
// }
// void MemBank::OnUpdate(float fElapsedTime) 
// {
//     // printf("%s::OnUpdate()\n", Name().c_str());    
// }
// void MemBank::OnRender() 
// {
//     // printf("%s::OnRender()\n", Name().c_str());    
// }

