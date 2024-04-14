// *************************************************
// *
// * Memory.cpp
// *
// ***********************************

#include "Memory.hpp"
#include "Bus.hpp"
#include "MemBank.hpp"

Byte Memory::read(Word offset, bool debug) 
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    MemBank* mb = Bus::GetMemBank();
    switch (offset)
    {
        case MEM_BANK1_SELECT:  data = mb->get_bank_1_page(); break; 
        case MEM_BANK2_SELECT:  data = mb->get_bank_2_page(); break;  
        case MEM_BANK1_TYPE:    data = mb->get_bank_1_type(); break; 
        case MEM_BANK2_TYPE:    data = mb->get_bank_2_type(); break; 
        case MEM_EXT_ADDR:      break; 
        case MEM_EXT_PITCH:     break; 
        case MEM_EXT_WIDTH:     break; 
        case MEM_EXT_DATA:      break;        
    }

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}

void Memory::write(Word offset, Byte data, bool debug) 
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    MemBank* mb = Bus::GetMemBank();
    switch (offset)
    {
        case MEM_BANK1_SELECT:  { 
            mb->set_bank_1_page(data);
            data = mb->get_bank_1_page();
            break; 
        }
        case MEM_BANK2_SELECT:  { 
            mb->set_bank_2_page(data);
            data = mb->get_bank_2_page();
            break; 
        }
        case MEM_BANK1_TYPE:    { 
            mb->set_bank_1_type((MemBank::BANK_TYPE)data);
            data = mb->get_bank_1_type();
            break; 
        }  
        case MEM_BANK2_TYPE:    { 
            mb->set_bank_2_type((MemBank::BANK_TYPE)data);
            data = mb->get_bank_2_type();
            break; 
        }
        case MEM_EXT_ADDR:      break; 
        case MEM_EXT_PITCH:     break; 
        case MEM_EXT_WIDTH:     break; 
        case MEM_EXT_DATA:      break;        
    }
    IDevice::write(offset,data);   // update any internal changes too
}

Word Memory::OnAttach(Word nextAddr) 
{
    // printf("%s::OnAttach()\n", Name().c_str());    
    Word old_addr = nextAddr;
    Byte enumID = 0;

    DisplayEnum("", 0,                          "Memory Device Hardware Registers");
    DisplayEnum("MEM_BEGIN",        nextAddr,   " Start of Memory Device Hardware Registers");  nextAddr++;
    DisplayEnum("MEM_BANK1_SELECT", nextAddr,   "(Byte) select 8k page for bank 1 (0-255)");    nextAddr++;
    DisplayEnum("MEM_BANK2_SELECT", nextAddr,   "(Byte) select 8k page for bank 2 (0-255)");    nextAddr++;
    DisplayEnum("MEM_BANK1_TYPE",   nextAddr,   "(Byte) memory bank 1 type");                   nextAddr++;
    DisplayEnum("MEM_BANK2_TYPE",   nextAddr,   "(Byte) memory bank 2 type");                   nextAddr++;
	DisplayEnum("MEM_TYPE_RAM",     enumID++,   "     random access memory (RAM)");
	DisplayEnum("MEM_TYPE_PERSIST", enumID++,   "     persistent memory (saved RAM)");
	DisplayEnum("MEM_TYPE_ROM",     enumID++,   "     read only memory (ROM)");
    DisplayEnum("MEM_EXT_ADDR",     nextAddr,   "(Word) Extended Memory Address Port");         nextAddr+=2;
    DisplayEnum("MEM_EXT_PITCH",    nextAddr,   "(Word) number of bytes per line");             nextAddr+=2;
    DisplayEnum("MEM_EXT_WIDTH",    nextAddr,   "(Word) width before skipping to next line");   nextAddr+=2;
    DisplayEnum("MEM_EXT_DATA",     nextAddr,   "(Byte) External Memory Data Port");            nextAddr++;
    DisplayEnum("MEM_END", nextAddr,            " End of Memory Device Hardware Registers");
    DisplayEnum("", 0, "");

    return nextAddr - old_addr;
}
void Memory::OnInit() 
{
    // printf("%s::OnInit()\n", Name().c_str());    
}
void Memory::OnQuit() 
{
    // printf("%s::OnQuit()\n", Name().c_str());    
}
void Memory::OnActivate() 
{
    // printf("%s::OnActivate()\n", Name().c_str());    
}
void Memory::OnDeactivate() 
{
    // printf("%s::OnDeactivate()\n", Name().c_str());    
}
void Memory::OnEvent(SDL_Event* evnt) 
{
    // printf("%s::OnEvent()\n", Name().c_str());    
}
void Memory::OnUpdate(float fElapsedTime) 
{
    // printf("%s::OnUpdate()\n", Name().c_str());    
}
void Memory::OnRender() 
{
    // printf("%s::OnRender()\n", Name().c_str());    
}

