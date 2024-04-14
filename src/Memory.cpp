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

    MemBank* mb = Bus::GetMemBank();    // fetch a pointer to the MemBank device
    switch (offset)
    {
        case MEM_BANK1_SELECT:  data = mb->get_bank_1_page();   break; 
        case MEM_BANK2_SELECT:  data = mb->get_bank_2_page();   break;  
        case MEM_BANK1_TYPE:    data = mb->get_bank_1_type();   break; 
        case MEM_BANK2_TYPE:    data = mb->get_bank_2_type();   break; 
        case MEM_EXT_ADDR+0:    data =  (reg_addr>>8) & 0xFF;   break; 
        case MEM_EXT_ADDR+1:    data =  (reg_addr>>0) & 0xFF;   break; 
        case MEM_EXT_PITCH+0:   data = (reg_pitch>>8) & 0xFF;   break; 
        case MEM_EXT_PITCH+1:   data = (reg_pitch>>0) & 0xFF;   break; 
        case MEM_EXT_WIDTH+0:   data = (reg_width>>8) & 0xFF;   break; 
        case MEM_EXT_WIDTH+1:   data = (reg_width>>0) & 0xFF;   break; 
        case MEM_EXT_DATA:      data = ext_memory[reg_addr];    break;                

        case MEM_DYN_SIZE+0:    data =  (reg_size>>8) & 0xFF;   break; 
        case MEM_DYN_SIZE+1:    data =  (reg_size>>0) & 0xFF;   break; 

        case MEM_DYN_ADDR+0:    data =  (reg_address>>8) & 0xFF;   break; 
        case MEM_DYN_ADDR+1:    data =  (reg_address>>0) & 0xFF;   break; 

        case MEM_DYN_AVAIL+0:    
            reg_avail = _memAvail(); 
            data =  (reg_avail>>8) & 0xFF;   
            break; 
        case MEM_DYN_AVAIL+1:    
            reg_avail = _memAvail(); 
            data =  (reg_avail>>0) & 0xFF;   
            break; 
    }

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}

void Memory::write(Word offset, Byte data, bool debug) 
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    MemBank* mb = Bus::GetMemBank();    // fetch a pointer to the MemBank device
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
        case MEM_EXT_ADDR+0:    reg_addr =  (reg_addr  & 0x00FF) | (data << 8); break; 
        case MEM_EXT_ADDR+1:    reg_addr =  (reg_addr  & 0xFF00) | (data << 0); break; 
        case MEM_EXT_PITCH+0:   reg_pitch = (reg_pitch & 0x00FF) | (data << 8); break; 
        case MEM_EXT_PITCH+1:   reg_pitch = (reg_pitch & 0xFF00) | (data << 0); break; 
        case MEM_EXT_WIDTH+0:   reg_width = (reg_width & 0x00FF) | (data << 8); break; 
        case MEM_EXT_WIDTH+1:   reg_width = (reg_width & 0xFF00) | (data << 0); break; 
        case MEM_EXT_DATA:      
        {
            static Word s_width = reg_width+1;
            ext_memory[reg_addr] = data;
            reg_addr++;
            if (--s_width==0)
            {
                s_width = reg_width;
                reg_addr += (reg_pitch - reg_width);
            }
            // printf("Memory::MEM_EXT_DATA($%04x, $%02X)\n", reg_addr, data);
            break;        
        }

        case MEM_DYN_SIZE+0:    reg_size =  (reg_size  & 0x00FF) | (data << 8); break;
        case MEM_DYN_SIZE+1:    
            reg_size =  (reg_size  & 0xFF00) | (data << 0);
            _onSizeLSB();    
            break;

        case MEM_DYN_ADDR+0:    reg_address =  (reg_address  & 0x00FF) | (data << 8); break;
        case MEM_DYN_ADDR+1:    reg_address =  (reg_address  & 0xFF00) | (data << 0); break;

        case MEM_DYN_AVAIL+0:    break;     // READ ONLY
        case MEM_DYN_AVAIL+1:    break;     // READ ONLY
    }
    // bounds checking
    if (reg_pitch == 0)    reg_pitch = 1;
    if (reg_width == 0)    reg_width = 1;

    // update any internal changes too
    IDevice::write(offset,data);   
}

Word Memory::OnAttach(Word nextAddr) 
{
    // printf("%s::OnAttach()\n", Name().c_str());    
    Word old_addr = nextAddr;
    Byte enumID = 0;

    DisplayEnum("", 0,                          "Memory Device Hardware Registers");
    DisplayEnum("MEM_BEGIN",        nextAddr,   "Start of Memory Device Hardware Registers");   nextAddr++;
    DisplayEnum("MEM_BANK1_SELECT", nextAddr,   "(Byte) select 8k page for bank 1 (0-255)");    nextAddr++;
    DisplayEnum("MEM_BANK2_SELECT", nextAddr,   "(Byte) select 8k page for bank 2 (0-255)");    nextAddr++;
    DisplayEnum("MEM_BANK1_TYPE",   nextAddr,   "(Byte) memory bank 1 type");                   nextAddr++;
    DisplayEnum("MEM_BANK2_TYPE",   nextAddr,   "(Byte) memory bank 2 type");                   nextAddr++;
	DisplayEnum("MEM_TYPE_RAM",     enumID++,   "     random access memory (RAM)");
	DisplayEnum("MEM_TYPE_PERSIST", enumID++,   "     persistent memory (saved RAM)");
	DisplayEnum("MEM_TYPE_ROM",     enumID++,   "     read only memory (ROM)");
    DisplayEnum("", 0, "");
    DisplayEnum("MEM_EXT_ADDR",     nextAddr,   "(Word) Extended Memory Address Port");         nextAddr+=2;
    DisplayEnum("MEM_EXT_PITCH",    nextAddr,   "(Word) number of bytes per line");             nextAddr+=2;
    DisplayEnum("MEM_EXT_WIDTH",    nextAddr,   "(Word) width before skipping to next line");   nextAddr+=2;
    DisplayEnum("MEM_EXT_DATA",     nextAddr,   "(Byte) External Memory Data Port");            nextAddr++;
    DisplayEnum("", 0, "");
    DisplayEnum("MEM_DYN_SIZE",     nextAddr,   "(Word) dynamic memory block size");            nextAddr+=2;
	DisplayEnum("",                         0,  "     Notes: Memory allocation occurs when the ");
	DisplayEnum("",                         0,  "            least-significant byte is written.");
	DisplayEnum("",                         0,  "            Reads as total number of bytes allocated");
	DisplayEnum("",                         0,  "            or freed. When $0000 is written to this ");
    DisplayEnum("",                         0,  "            port, memory node at MEM_DYN_ADDR is freed.");
    DisplayEnum("MEM_DYN_ADDR",     nextAddr,   "(Word) address of a dynamic memory node");     nextAddr+=2;
    DisplayEnum("MEM_DYN_AVAIL",    nextAddr,   "(Word) number of non-allocated bytes");        nextAddr+=2;

    DisplayEnum("MEM_END", nextAddr,            "End of Memory Device Hardware Registers");
    DisplayEnum("", 0, "");

    return nextAddr - old_addr;
}

// return a pointer to the first block of SIZE available on the heap
Word Memory::_findFirstBlockOfSize(Word p_size)
{
    int addr = 0xFFFF;
    static int s_lastAddr = addr;
    int gap = 0;
    int size = 0;
    
    std::map<Word,Word>::reverse_iterator itr;
    for (itr = dyn_heap.rbegin(); itr != dyn_heap.rend(); itr++)
    {        
        addr = itr->first;
        size = itr->second;
        gap = s_lastAddr - addr;
        // printf("GAP of %d at $%04X\n", gap, s_lastAddr);
        if (gap >= p_size)
        {
            // printf("GAP of %d at $%04X\n", gap, s_lastAddr);
            return s_lastAddr;
        }
        s_lastAddr = addr-size;       
        // printf("[$%04X].size=%d\n", addr, size);
    }
    addr = s_lastAddr;
    if ((addr - p_size) < memory_btm)    return 0;      // out of memory error
    // printf("FALL THROUGH: $%04X\n", addr);
    return addr;
}

// fires when the LSB of the MEM_DYN_SIZE register is written to
void Memory::_onSizeLSB()
{
    // printf("Memory::_onSizeLSB() size: $%04X\n", _findFirstBlockOfSize(reg_size));

    // case: when $0000 is written to 'reg_size'...  FREE memory block at 'reg_address'
    if (reg_size == 0)
    {
        // free the block at 'reg_address'
        // printf("Free the block at $%04X\n", reg_address);
        if (dyn_heap[reg_address] != 0)
        {
            // return number of bytes freed
            reg_size = dyn_heap[reg_address];
            // dyn_heap[reg_address] = 0;
            dyn_heap.erase(reg_address);
        }
    }    
    else    // case: when non-zero is written to MEM_DYN_SIZE... ALLOCATE a memory block
    {
        Word new_addr = _findFirstBlockOfSize(reg_size);
        dyn_heap[new_addr] = reg_size;
        reg_address = new_addr;
        // printf("Allocate $%04X bytes at $%04X\n", reg_size, reg_address);
    }
    // // display the heap table
    // printf("HEAP TABLE (%d):\n", (int)dyn_heap.size());
    // std::map<Word,Word>::reverse_iterator itr;
    // for (itr = dyn_heap.rbegin(); itr != dyn_heap.rend(); itr++)
    // {
    //     printf("[$%04X] size: %d\n", itr->first, itr->second);
    // }
}

// return the number of unallocated bytes on the heap
Word Memory::_memAvail()
{
    return 0xCCAA;
}