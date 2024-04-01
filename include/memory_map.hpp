
// memory_map.h
#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

enum MEMMAP
{
    //  **********************************************
    //  * Allocated 64k Memory Mapped System Symbols *
    //  **********************************************
    SOFT_VECTORS     = 0x0000, 
        
        // Software Interrupt Vectors:
    SOFT_EXEC        = 0x0000, // Exec Software Interrupt Vector
    SOFT_SWI3        = 0x0002, // SWI3 Software Interrupt Vector
    SOFT_SWI2        = 0x0004, // SWI2 Software Interrupt Vector
    SOFT_FIRQ        = 0x0006, // FIRQ Software Interrupt Vector
    SOFT_IRQ         = 0x0008, // IRQ Software Interrupt Vector
    SOFT_SWI         = 0x000A, // SWI / SYS Software Interrupt Vector
    SOFT_NMI         = 0x000C, // NMI Software Interrupt Vector
    SOFT_RESET       = 0x000E, // RESET Software Interrupt Vector
        
        // System Memory:
    ZERO_PAGE        = 0x0010, 
    USER_STACK       = 0x0100, 
    USTACK_TOP       = 0x0200, // Top of the user stack space
    SYSTEM_STACK     = 0x0200, 
    SSTACK_TOP       = 0x0400, // Top of the system stack space
        
        // Video Buffer (11K)
    VIDEO_START      = 0x0400, // Start of Video Buffer Memory
    VIDEO_END        = 0x2FFF, // End of Video Buffer Memory
        
        // User RAM (32K)
    USER_RAM         = 0x3000, 
        
        // Paged Memory Bank One (8K)
    PG_BANK_ONE      = 0xB000, 
        
        // Paged Memory Bank Two (8K)
    PG_BANK_TWO      = 0xD000, 
        
        // KERNEL ROM (3.5K)
    KERNEL_ROM       = 0xF000, 
        
        // HARDWARE REGISTERS (0.5K)
    GFX_MODE         = 0xFE00, // (Byte) Graphics Mode
        //           - bit 0-4   = Resolution Modes 0-31
        //           - bit 7     = 0:text,  1:bitmap
        
    GFX_EMU          = 0xFE01, // (Byte) Emulation Flags
        //           - bits 0-2  = Active Monitor 0-7
        //           - bits 3-5  = reserved
        //           - bit  6    = 0:vsync off, 1:vsync on
        //           - bit  7    = 0:windowed, 1:fullscreen
        
        
        // 494 bytes in reserve
    RESERVED         = 0xFE02, 
        
        // Hardware Interrupt Vectors:
    ROM_VECTS        = 0xFFF0, 
    HARD_EXEC        = 0xFFF0, // EXEC Hardware Interrupt Vector
    HARD_SWI3        = 0xFFF2, // SWI3 Hardware Interrupt Vector
    HARD_SWI2        = 0xFFF4, // SWI2 Hardware Interrupt Vector
    HARD_FIRQ        = 0xFFF6, // FIRQ Hardware Interrupt Vector
    HARD_IRQ         = 0xFFF8, // IRQ Hardware Interrupt Vector
    HARD_SWI         = 0xFFFA, // SWI / SYS Hardware Interrupt Vector
    HARD_NMI         = 0xFFFC, // NMI Hardware Interrupt Vector
    HARD_RESET       = 0xFFFE, // RESET Hardware Interrupt Vector
};  // END: enum MEMMAP


#endif // __MEMORY_MAP_H__


