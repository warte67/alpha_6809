
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
        
        // Video Buffer (15K)
    VIDEO_START      = 0x0400, 
    VIDEO_END        = 0x3FFF, // End of Default Video Space
        
        // User Ram (28K)
    USER_RAM         = 0x4000, 
        
        // Paged Memory Bank One (8K)
    PG_BANK_ONE      = 0xB000, 
        
        // Paged Memory Bank Two (8K)
    PG_BANK_TWO      = 0xD000, 
        
        // KERNEL ROM (3.5K)
    KERNEL_ROM       = 0xF000, 
        
        // HARDWARE REGISTERS (0.5K)
    GFX_BEGIN        = 0xFE00, //  Start of Graphics Hardware Registers
    GFX_MODE         = 0xFE00, // (Byte) Graphics Mode
        //           - bit 0-4   = Resolution Modes 0-31
        //           - bit 7     = 0:text,  1:bitmap
        
    GFX_EMU          = 0xFE01, // (Byte) Emulation Flags
        //           - bits 0-2  = Active Monitor 0-7
        //           - bits 3-5  = Debug Monitor 0-7
        //           - bit  6    = 0:vsync off, 1:vsync on
        //           - bit  7    = 0:windowed, 1:fullscreen
        
    GFX_VID_END      = 0xFE02, //  (Word Read Only) Top of Display Buffer
        // Note: This will change to reflect the highest address of 
        //     the currently running video display mode.
        
    GFX_HRES         = 0xFE04, //  (Word Read Only) Horizontal Display Resolution
        // Note: This will reflect the number of character columns for the 
        //     text modes, but will reflect pixel columns for bitmap modes. 
        
    GFX_VRES         = 0xFE06, //  (Word Read Only) Vertical Display Resolution
        // Note: This will reflect the number of character rows for the 
        //     text modes, but will reflect pixel rows for bitmap modes. 
        
    GFX_PAL_IDX      = 0xFE08, //  (Byte) Color Palette Index
        // GFX_PAL_IDX: 0-255
        // Note: Use this register to set the index into theColor Palette. 
        //       Set this value prior referencing color data (GFX_PAL_CLR).
        
    GFX_PAL_CLR      = 0xFE09, //  (Word) Indexed Color Palette Data
        // GFX_PAL_CLR: Color Data A4R4G4B4 format
        // Note: This is the color data for an individual palette entry.
        //     Write to DSP_PAL_IDX with the index within the color palette
        //     prior to reading or writing the color data in the GFX_PAL_CLR register.
        
    GFX_GLYPH_IDX    = 0xFE0B, //  (Byte) Text Glyph Index
        // GFX_GLYPH_IDX: 0-256
        // Note: Set this register to index a specific text glyph. Set this value
        //     prior to updating glyph pixel data.
        
    GFX_GLYPH_DATA   = 0xFE0C, //  (8-Bytes) Text Glyph Pixel Data Array
        // GFX_GLYPH_DATA: 8 rows of binary encoded glyph pixel data
        // Note: Each 8x8 text glyph is composed of 8 bytes. The first byte in this 
        //     array represents the top line of 8 pixels. Each array entry represents
        //     a row of 8 pixels. 
        
    GFX_END          = 0xFE14, //  End of Graphics Hardware Registers
        
        // System Hardware Registers:
        
    SYS_BEGIN        = 0xFE14, //  Start of System Hardware Registers
    SYS_STATE        = 0xFE14, //  (Byte) System State Register
        // SYS_STATE: ABCD.SSSS
        //      A:0   = Error: Standard Buffer Overflow 
        //      B:0   = Error: Extended Buffer Overflow 
        //      C:0   = Error: Reserved 
        //      D:0   = Error: Reserved 
        //      S:$0  = CPU Clock  25 khz.
        //      S:$1  = CPU Clock  50 khz.
        //      S:$2  = CPU Clock 100 khz.
        //      S:$3  = CPU Clock 200 khz.
        //      S:$4  = CPU Clock 333 khz.
        //      S:$5  = CPU Clock 416 khz.
        //      S:$6  = CPU Clock 500 khz.
        //      S:$7  = CPU Clock 625 khz.
        //      S:$8  = CPU Clock 769 khz.
        //      S:$9  = CPU Clock 833 khz.
        //      S:$A  = CPU Clock 1.0 mhz.
        //      S:$B  = CPU Clock 1.4 mhz.
        //      S:$C  = CPU Clock 2.0 mhz.
        //      S:$D  = CPU Clock 3.3 mhz.
        //      S:$E  = CPU Clock 5.0 mhz.
        //      S:$F  = CPU Clock ~10.0 mhz. (unmetered)
        
    SYS_SPEED        = 0xFE15, //  (Word) Approx. Average CPU Clock Speed
    SYS_CLOCK_DIV    = 0xFE17, //  (Byte) 60 hz Clock Divider Register (Read Only) 
        // SYS_CLOCK_DIV:
        //      bit 7: 0.46875 hz
        //      bit 6: 0.9375 hz
        //      bit 5: 1.875 hz
        //      bit 4: 3.75 hz
        //      bit 3: 7.5 hz
        //      bit 2: 15.0 hz
        //      bit 1: 30.0 hz
        //      bit 0: 60.0 hz
        
    SYS_TIMER        = 0xFE18, //  (Word) Increments at 0.46875 hz
    SYS_END          = 0xFE1A, //  End of System Hardware Registers
        
        // Debug Hardware Registers:
    DBG_BEGIN        = 0xFE1A, //  start of debugger hardware registers
    DBG_BRK_ADDR     = 0xFE1A, //    (Word) Address of current breakpoint
    DBG_FLAGS        = 0xFE1C, //    (Byte) Debug Specific Hardware Flags:
        //     bit 7: Debug Enable
        //     bit 6: Single Step Enable
        //     bit 5: Clear All Breakpoints
        //     bit 4: Update Breakpoint at DEBUG_BRK_ADDR
        //     bit 3: FIRQ  (on low to high edge)
        //     bit 2: IRQ   (on low to high edge)
        //     bit 1: NMI   (on low to high edge)
        //     bit 0: RESET (on low to high edge)
    DBG_END          = 0xFE1D, // End Debug Registers
        
        
        // Mouse Cursor Hardware Registers:
    CSR_BEGIN        = 0xFE1D, //  Start of Mouse Cursor Hardware Registers
    CSR_XPOS         = 0xFE1D, //  (Word) horizontal mouse cursor coordinate
    CSR_YPOS         = 0xFE1F, //  (Word) vertical mouse cursor coordinate
    CSR_XOFS         = 0xFE21, //  (Byte) horizontal mouse cursor offset
    CSR_YOFS         = 0xFE22, //  (Byte) vertical mouse cursor offset
    CSR_SCROLL       = 0xFE23, //  (Signed) MouseWheel Scroll: -1, 0, 1
    CSR_FLAGS        = 0xFE24, //  (Byte) mouse button flags:
        //  CSR_FLAGS:
        //       bits 0-4: button states
        //       bits 5-6: number of clicks
        //       bits 7:   cursor enable
    CSR_BMP_INDX     = 0xFE25, //  (Byte) mouse cursor bitmap pixel offset
    CSR_BMP_DATA     = 0xFE26, //  (Byte) mouse cursor bitmap pixel index color
    CSR_PAL_INDX     = 0xFE28, //  (Byte) mouse cursor color palette index (0-15)
    CSR_PAL_DATA     = 0xFE29, //  (Word) mouse cursor color palette data RGBA4444
    CSR_END          = 0xFE2B, // End Mouse Registers
        
    RESERVED         = 0xFE2B, 
        // 453 bytes in reserve
        
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


