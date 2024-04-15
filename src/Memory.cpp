// *************************************************
// *
// * Memory.cpp
// *
// ***********************************

#include "Memory.hpp"
#include "Bus.hpp"
#include "MemBank.hpp"
#include "Gfx.hpp"

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

        case MEM_DSP_FLAGS:     data = reg_dsp_flags;           break;
        case MEM_DSPLY_SIZE+0:  data =  (memory_btm>>8) & 0xFF;     break; 
        case MEM_DSPLY_SIZE+1:  data =  (memory_btm>>0) & 0xFF;     break; 

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
            reg_avail = MemAvailable(); 
            data =  (reg_avail>>8) & 0xFF;   
            break; 
        case MEM_DYN_AVAIL+1:    
            reg_avail = MemAvailable(); 
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
        case MEM_DSP_FLAGS:         
        {
            Gfx* gfx = Bus::GetGfx();
            if (data & 0x80)
            {
                // calc the display buffer size
                Byte bpp = 1<<(data & 3);
                int size = ((gfx->res_width * gfx->res_height) / 8) * bpp;
                memory_btm = size;
                while (size > 0xffff)
                {
                    //Bus::Write(MEM_DSP_FLAGS, ((Bus::Read(MEM_DSP_FLAGS)-1)&3)|0x80);
                    data = (data-1) & 3;
                    bpp = 1<<(data&3);
                    size = ((gfx->res_width * gfx->res_height) / 8) * bpp;
                    memory_btm = size;
                }
                data |= 0x80;
                // printf("BPP:%d  SIZE:$%04X\n", bpp, size);
            }            
            reg_dsp_flags = data & 0x83;   
            break;
        }
        case MEM_DSPLY_SIZE+0:      memory_btm =  (memory_btm  & 0x00FF) | (data << 8); break; 
        case MEM_DSPLY_SIZE+1:      memory_btm =  (memory_btm  & 0xFF00) | (data << 0); break; 
        case MEM_EXT_ADDR+0:        reg_addr =  (reg_addr  & 0x00FF) | (data << 8); break; 
        case MEM_EXT_ADDR+1:        reg_addr =  (reg_addr  & 0xFF00) | (data << 0); break; 
        case MEM_EXT_PITCH+0:       reg_pitch = (reg_pitch & 0x00FF) | (data << 8); break; 
        case MEM_EXT_PITCH+1:       reg_pitch = (reg_pitch & 0xFF00) | (data << 0); break; 
        case MEM_EXT_WIDTH+0:       reg_width = (reg_width & 0x00FF) | (data << 8); break; 
        case MEM_EXT_WIDTH+1:       reg_width = (reg_width & 0xFF00) | (data << 0); break; 
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
    DisplayEnum("MEM_DSP_FLAGS",    nextAddr,   "(Byte) Extended Graphics Display Flags");      nextAddr++;
	DisplayEnum("",                         0,  "     bit 7:    0=standard graphics, 1:extended graphics ");
	DisplayEnum("",                         0,  "     bits 2-6: reserved (possibly for tilemap support)");
	DisplayEnum("",                         0,  "     bits 0-1: color depth:  ");
    DisplayEnum("",                         0,  "           0:2-color, 1:4-color, 2:16-color, 3:256-color");
    DisplayEnum("MEM_DISPLAY_SIZE", nextAddr,   "(Word) Extended Graphics Buffer Size");        nextAddr+=2;
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
    // s_lastAddr = addr;
    int gap = 0;
    int size = 0;
    
    std::map<Word,Word>::reverse_iterator itr;
    for (itr = dyn_heap.rbegin(); itr != dyn_heap.rend(); itr++)
    {        
        addr = itr->first;
        size = itr->second;
        if (addr-size < MemAvailable())  return 0;       // out of memory error
        gap = s_lastAddr - addr;
        // printf("GAP of %d at $%04X\n", gap, s_lastAddr);
        // if (s_lastAddr <= memory_btm)   break;       // out of memory error
        if (gap >= p_size)
        {
            // printf("GAP of %d at $%04X\n", gap, s_lastAddr);
            return s_lastAddr;
        }
        s_lastAddr = addr-size;       
        // printf("[$%04X].size=%d\n", addr, size);
    }
    addr = s_lastAddr;
    // if ((addr - p_size) < memory_btm)    return 0;      // out of memory error
    // printf("FALL THROUGH: $%04X\n", addr);
    return addr;
}

// fires when the LSB of the MEM_DYN_SIZE register is written to
void Memory::_onSizeLSB()
{
    // printf("Memory::_onSizeLSB() size: $%04X\n", _findFirstBlockOfSize(reg_size));

    // case: when $0000 is written to 'reg_size'...  FREE memory block at 'reg_address'
    if (reg_size == 0)
        reg_size = MemDelete(reg_address);
    else    // case: when non-zero is written to MEM_DYN_SIZE... ALLOCATE a memory block
        reg_size = MemAlloc(reg_size);

    if (false)  // display the heap table
    {
        printf("HEAP TABLE (%d):\n", (int)dyn_heap.size());
        std::map<Word,Word>::reverse_iterator itr;
        for (itr = dyn_heap.rbegin(); itr != dyn_heap.rend(); itr++)
        {
            printf("[$%04X] size: %d\n", itr->first, itr->second);
        }
    }
}

// returns address of the block in the extended heap
Word Memory::MemAlloc(Word size)
{

    Word new_addr = _findFirstBlockOfSize(reg_size);
    if (new_addr != 0)
    {    
        dyn_heap[new_addr] = reg_size;
        reg_address = new_addr;
    }
    else
    {
        reg_size = 0;
        Bus::Write(SYS_STATE, Bus::Read(SYS_STATE) | 0x40); // set the extended buffer overflow error bit
        // Bus::Error("Out of Extended Memory");            
    }
    return reg_size;
}

// frees memory and returns number of bytes de-allocated
Word Memory::MemDelete(Word addr)
{
    if (dyn_heap[reg_address] != 0)
    {
        // return number of bytes freed
        reg_size = dyn_heap[reg_address];
        // dyn_heap[reg_address] = 0;
        dyn_heap.erase(reg_address);
    }
    return reg_size;
}  

// return the number of unallocated bytes on the heap
Word Memory::MemAvailable()
{
    int mem_size = 0xFFFF - memory_btm;
    for (auto& [addr,size] : dyn_heap)
    {
        if (mem_size - size >= 0)
            mem_size -= size;
        else
            break;
    }

    return (Word)mem_size;
}

void Memory::OnInit() 
{
    LoadBMP("scene_c.bmp");
}

void Memory::OnQuit() 
{
    // ...
}



void Memory::OnUpdate(float fElapsedTime)
{
    // TESTING:  For now, just fill the extended video buffer with static every frame
    if (false)
    {
        if (Bus::Read(MEM_DSP_FLAGS) & 0x80)
        {
            Bus::Write_Word(MEM_EXT_ADDR,0);
            Bus::Write_Word(MEM_EXT_PITCH,1);
            Bus::Write_Word(MEM_EXT_WIDTH,1);
            for (int t=0; t<memory_btm; t++)
                Bus::Write(MEM_EXT_DATA,rand()%256);
            Bus::Write_Word(MEM_EXT_ADDR,0);
        }
    }
    // END TESTING...
}

bool Memory::LoadBMP(const std::string& file)
{
    const char *image_path = file.c_str();
    SDL_Surface *image = SDL_LoadBMP(image_path);

    /* Let the user know if the file failed to load */
    if (!image) {
        printf("Failed to load image at %s: %s\n", image_path, SDL_GetError());
        return false;
    }

    // read the pixel data from the surface
    SDL_LockSurface(image);
    Bus::Write_Word(MEM_EXT_ADDR,   0x0000);
    Bus::Write_Word(MEM_EXT_PITCH,  0x0001);
    Bus::Write_Word(MEM_EXT_WIDTH,  0x0001);
    Byte* pixels = (Byte*)image->pixels;
    int bpp = image->format->BytesPerPixel;
    for(int y = 0; y < image->h; y++)
        for(int x = 0; x < image->w; x++)
             Bus::Write(MEM_EXT_DATA, pixels[(image->pitch * y) + (x * bpp)]);

    // read the palette data from the image
    if(image->format->BitsPerPixel==8)
    {
        for (int idx=0; idx<256; idx++)
        {
            SDL_Color *color=&image->format->palette->colors[idx];
            Word MSB = 0xF000 | ((color->r)>>4)<<8;
            Word LSB = 0x0000 | (color->g & 0xF0);
            LSB |= ((color->b)>>4);

            Word data = MSB | LSB;
            Bus::Write(GFX_PAL_IDX, (Byte)idx);
            Bus::Write_Word(GFX_PAL_CLR, data);
            printf("Pixel Color $%04X, Red: %3d, Green: %3d, Blue: %3d, Index: %3d\n",
                data, color->r, color->g, color->b, idx);
        }
    }    
    SDL_UnlockSurface(image);

    /* Make sure to eventually release the surface resource */
    SDL_FreeSurface(image);
    return true;
}
