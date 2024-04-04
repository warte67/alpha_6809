
; memory_map.asm
;  **********************************************
;  * Allocated 64k Memory Mapped System Symbols *
;  **********************************************
SOFT_VECTORS        equ   $0000  
        
          ; Software Interrupt Vectors:
SOFT_EXEC           equ   $0000    ; Exec Software Interrupt Vector
SOFT_SWI3           equ   $0002    ; SWI3 Software Interrupt Vector
SOFT_SWI2           equ   $0004    ; SWI2 Software Interrupt Vector
SOFT_FIRQ           equ   $0006    ; FIRQ Software Interrupt Vector
SOFT_IRQ            equ   $0008    ; IRQ Software Interrupt Vector
SOFT_SWI            equ   $000A    ; SWI / SYS Software Interrupt Vector
SOFT_NMI            equ   $000C    ; NMI Software Interrupt Vector
SOFT_RESET          equ   $000E    ; RESET Software Interrupt Vector
        
          ; System Memory:
ZERO_PAGE           equ   $0010  
USER_STACK          equ   $0100  
USTACK_TOP          equ   $0200    ; Top of the user stack space
SYSTEM_STACK        equ   $0200  
SSTACK_TOP          equ   $0400    ; Top of the system stack space
        
          ; Video Buffer (15K)
VIDEO_START         equ   $0400  
VIDEO_END           equ   $3FFF    ; End of Default Video Space
        
          ; User Ram (28K)
USER_RAM            equ   $4000  
        
          ; Paged Memory Bank One (8K)
PG_BANK_ONE         equ   $B000  
        
          ; Paged Memory Bank Two (8K)
PG_BANK_TWO         equ   $D000  
        
          ; KERNEL ROM (3.5K)
KERNEL_ROM          equ   $F000  
        
          ; HARDWARE REGISTERS (0.5K)
GFX_MODE            equ   $FE00    ; (Byte) Graphics Mode
          ;          - bit 0-4   = Resolution Modes 0-31
          ;          - bit 7     = 0:text,  1:bitmap
        
GFX_EMU             equ   $FE01    ; (Byte) Emulation Flags
          ;          - bits 0-2  = Active Monitor 0-7
          ;          - bits 3-5  = reserved
          ;          - bit  6    = 0:vsync off, 1:vsync on
          ;          - bit  7    = 0:windowed, 1:fullscreen
        
GFX_VID_END         equ   $FE02    ;  (Word Read Only) Top of Display Buffer
          ; Note: This will change to reflect the highest address of 
          ;     the currently running video display mode.
        
GFX_HRES            equ   $FE04    ;  (Word Read Only) Horizontal Display Resolution
          ; Note: This will reflect the number of character columns for the 
          ;     text modes, but will reflect pixel columns for bitmap modes. 
        
GFX_VRES            equ   $FE06    ;  (Word Read Only) Vertical Display Resolution
          ; Note: This will reflect the number of character rows for the 
          ;     text modes, but will reflect pixel rows for bitmap modes. 
        
GFX_PAL_IDX         equ   $FE08    ;  (Byte) Color Palette Index
          ; GFX_PAL_IDX: 0-255
          ; Note: Use this register to set the index into theColor Palette. 
          ;       Set this value prior referencing color data (GFX_PAL_CLR).
        
GFX_PAL_CLR         equ   $FE09    ;  (Word) Indexed Color Palette Data
          ; GFX_PAL_CLR: Color Data A4R4G4B4 format
          ; Note: This is the color data for an individual palette entry.
          ;     Write to DSP_PAL_IDX with the index within the color palette
          ;     prior to reading or writing the color data in the GFX_PAL_CLR register.
        
GFX_GLYPH_IDX       equ   $FE0B    ;  (Byte) Text Glyph Index
          ; GFX_GLYPH_IDX: 0-256
          ; Note: Set this register to index a specific text glyph. Set this value
          ;     prior to updating glyph pixel data.
        
GFX_GLYPH_DATA      equ   $FE0C    ;  (8-Bytes) Text Glyph Pixel Data Array
          ; GFX_GLYPH_DATA: 8 rows of binary encoded glyph pixel data
          ; Note: Each 8x8 text glyph is composed of 8 bytes. The first byte in this 
          ;     array represents the top line of 8 pixels. Each array entry represents
          ;     a row of 8 pixels. 
        
          ; Debug Hardware Registers:
DBG_BEGIN           equ   $FE14    ; Start of Debug Hardware Registers
DBG_BRK_ADDR        equ   $FE14    ;    (Word) Address of current breakpoint
DBG_FLAGS           equ   $FE16    ;    (Byte) Debug Specific Hardware Flags:
          ;     bit 7: Debug Enable
          ;     bit 6: Single Step Enable
          ;     bit 5: Clear All Breakpoints
          ;     bit 4: Update Breakpoint at DEBUG_BRK_ADDR
          ;     bit 3: FIRQ  (on low to high edge)
          ;     bit 2: IRQ   (on low to high edge)
          ;     bit 1: NMI   (on low to high edge)
          ;     bit 0: RESET (on low to high edge)
DBG_END             equ   $FE17    ; End Debug Registers
        
RESERVED            equ   $FE17  
          ; 473 bytes in reserve
        
          ; Hardware Interrupt Vectors:
ROM_VECTS           equ   $FFF0  
HARD_EXEC           equ   $FFF0    ; EXEC Hardware Interrupt Vector
HARD_SWI3           equ   $FFF2    ; SWI3 Hardware Interrupt Vector
HARD_SWI2           equ   $FFF4    ; SWI2 Hardware Interrupt Vector
HARD_FIRQ           equ   $FFF6    ; FIRQ Hardware Interrupt Vector
HARD_IRQ            equ   $FFF8    ; IRQ Hardware Interrupt Vector
HARD_SWI            equ   $FFFA    ; SWI / SYS Hardware Interrupt Vector
HARD_NMI            equ   $FFFC    ; NMI Hardware Interrupt Vector
HARD_RESET          equ   $FFFE    ; RESET Hardware Interrupt Vector
; END of definitions



