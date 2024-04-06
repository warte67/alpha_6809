;  memory_map.asm

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
FIO_BUFFER          equ   $0100  
FIO_BFR_END         equ   $01FF    ; End of the File Input/Output Buffer
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
GFX_BEGIN           equ   $FE00    ;  Start of Graphics Hardware Registers
GFX_MODE            equ   $FE00    ; (Byte) Graphics Mode
          ;          - bit 0-4   = Resolution Modes 0-31
          ;          - bit 7     = 0:text,  1:bitmap
        
GFX_EMU             equ   $FE01    ; (Byte) Emulation Flags
          ;          - bits 0-2  = Active Monitor 0-7
          ;          - bits 3-5  = Debug Monitor 0-7
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
        
GFX_END             equ   $FE14    ;  End of Graphics Hardware Registers
        
          ; System Hardware Registers:
        
SYS_BEGIN           equ   $FE14    ;  Start of System Hardware Registers
SYS_STATE           equ   $FE14    ;  (Byte) System State Register
          ; SYS_STATE: ABCD.SSSS
          ;      A:0   = Error: Standard Buffer Overflow 
          ;      B:0   = Error: Extended Buffer Overflow 
          ;      C:0   = Error: Reserved 
          ;      D:0   = Error: Reserved 
          ;      S:$0  = CPU Clock  25 khz.
          ;      S:$1  = CPU Clock  50 khz.
          ;      S:$2  = CPU Clock 100 khz.
          ;      S:$3  = CPU Clock 200 khz.
          ;      S:$4  = CPU Clock 333 khz.
          ;      S:$5  = CPU Clock 416 khz.
          ;      S:$6  = CPU Clock 500 khz.
          ;      S:$7  = CPU Clock 625 khz.
          ;      S:$8  = CPU Clock 769 khz.
          ;      S:$9  = CPU Clock 833 khz.
          ;      S:$A  = CPU Clock 1.0 mhz.
          ;      S:$B  = CPU Clock 1.4 mhz.
          ;      S:$C  = CPU Clock 2.0 mhz.
          ;      S:$D  = CPU Clock 3.3 mhz.
          ;      S:$E  = CPU Clock 5.0 mhz.
          ;      S:$F  = CPU Clock ~10.0 mhz. (unmetered)
        
SYS_SPEED           equ   $FE15    ;  (Word) Approx. Average CPU Clock Speed
SYS_CLOCK_DIV       equ   $FE17    ;  (Byte) 60 hz Clock Divider Register (Read Only) 
          ; SYS_CLOCK_DIV:
          ;      bit 7: 0.46875 hz
          ;      bit 6: 0.9375 hz
          ;      bit 5: 1.875 hz
          ;      bit 4: 3.75 hz
          ;      bit 3: 7.5 hz
          ;      bit 2: 15.0 hz
          ;      bit 1: 30.0 hz
          ;      bit 0: 60.0 hz
        
SYS_TIMER           equ   $FE18    ;  (Word) Increments at 0.46875 hz
SYS_END             equ   $FE1A    ;  End of System Hardware Registers
        
          ; Debug Hardware Registers:
DBG_BEGIN           equ   $FE1A    ;  start of debugger hardware registers
DBG_BRK_ADDR        equ   $FE1A    ;    (Word) Address of current breakpoint
DBG_FLAGS           equ   $FE1C    ;    (Byte) Debug Specific Hardware Flags:
          ;     bit 7: Debug Enable
          ;     bit 6: Single Step Enable
          ;     bit 5: Clear All Breakpoints
          ;     bit 4: Update Breakpoint at DEBUG_BRK_ADDR
          ;     bit 3: FIRQ  (on low to high edge)
          ;     bit 2: IRQ   (on low to high edge)
          ;     bit 1: NMI   (on low to high edge)
          ;     bit 0: RESET (on low to high edge)
DBG_END             equ   $FE1D    ; End Debug Registers
        
          ; Mouse Cursor Hardware Registers:
CSR_BEGIN           equ   $FE1D    ;  Start of Mouse Cursor Hardware Registers
CSR_XPOS            equ   $FE1D    ;  (Word) horizontal mouse cursor coordinate
CSR_YPOS            equ   $FE1F    ;  (Word) vertical mouse cursor coordinate
CSR_XOFS            equ   $FE21    ;  (Byte) horizontal mouse cursor offset
CSR_YOFS            equ   $FE22    ;  (Byte) vertical mouse cursor offset
CSR_SCROLL          equ   $FE23    ;  (Signed) MouseWheel Scroll: -1, 0, 1
CSR_FLAGS           equ   $FE24    ;  (Byte) mouse button flags:
          ;  CSR_FLAGS:
          ;       bits 0-4: button states
          ;       bits 5-6: number of clicks
          ;       bits 7:   cursor enable
CSR_BMP_INDX        equ   $FE25    ;  (Byte) mouse cursor bitmap pixel offset
CSR_BMP_DATA        equ   $FE26    ;  (Byte) mouse cursor bitmap pixel index color
CSR_PAL_INDX        equ   $FE27    ;  (Byte) mouse cursor color palette index (0-15)
CSR_PAL_DATA        equ   $FE28    ;  (Word) mouse cursor color palette data RGBA4444
CSR_END             equ   $FE2A    ; End Mouse Registers
        
          ; Keyboard Hardware Registers:
KEY_BEGIN           equ   $FE2A    ; Start of the Keyboard Register space
CHAR_Q_LEN          equ   $FE2A    ;   (Byte) # of characters waiting in queue        (Read Only)
CHAR_SCAN           equ   $FE2B    ;   (Byte) read next character in queue (not popped when read)
CHAR_POP            equ   $FE2C    ;   (Byte) read next character in queue (popped when read)
XKEY_BUFFER         equ   $FE2D    ;   (128 bits) 16 bytes for XK_KEY data buffer     (Read Only)
EDT_BFR_CSR         equ   $FE3D    ;   (Byte) cursor position within edit buffer     (Read/Write)
EDT_ENABLE          equ   $FE3E    ;   (Byte) line editor enable flag                 (Read/Write)
EDT_BFR_LEN         equ   $FE3F    ;   (Byte) Limit the line editor to this length   (Read/Write)
EDT_BUFFER          equ   $FE40    ;   line editing character buffer                 (Read Only)
KEY_END             equ   $FF40    ; End of the Keyboard Register space
        
JOYS_BEGIN          equ   $FF40    ; Start of the Game Controller Register space
JOYS_1_BTN          equ   $FF40    ;   (Word) button bits: room for up to 16 buttons  (realtime)
JOYS_1_DBND         equ   $FF42    ;   (Byte) PAD 1 analog deadband; default is 5   (read/write)
JOYS_1_LTX          equ   $FF43    ;   (char) PAD 1 LThumb-X position (-128 _ +127)   (realtime)
JOYS_1_LTY          equ   $FF44    ;   (char) PAD 1 LThumb-Y position (-128 _ +127)   (realtime)
JOYS_1_RTX          equ   $FF45    ;   (char) PAD 1 RThumb-X position (-128 _ +127)   (realtime)
JOYS_1_RTY          equ   $FF46    ;   (char) PAD 1 RThumb-Y position (-128 _ +127)   (realtime)
JOYS_1_Z1           equ   $FF47    ;   (char) PAD 1 left analog trigger (0 - 127)     (realtime)
JOYS_1_Z2           equ   $FF48    ;   (char) PAD 1 right analog trigger (0 - 127)    (realtime)
JOYS_2_BTN          equ   $FF49    ;   (Word) button bits: room for up to 16 buttons  (realtime)
JOYS_2_DBND         equ   $FF4B    ;   (Byte) PAD 2 analog deadband; default is 5   (read/write)
JOYS_2_LTX          equ   $FF4C    ;   (char) PAD 2 LThumb-X position (-128 _ +127)   (realtime)
JOYS_2_LTY          equ   $FF4D    ;   (char) PAD 2 LThumb-Y position (-128 _ +127)   (realtime)
JOYS_2_RTX          equ   $FF4E    ;   (char) PAD 2 RThumb-X position (-128 _ +127)   (realtime)
JOYS_2_RTY          equ   $FF4F    ;   (char) PAD 2 RThumb-Y position (-128 _ +127)   (realtime)
JOYS_2_Z1           equ   $FF50    ;   (char) PAD 2 left analog trigger (0 - 127)     (realtime)
JOYS_2_Z2           equ   $FF51    ;   (char) PAD 2 right analog trigger (0 - 127)    (realtime)
JOYS_END            equ   $FF52    ; End of the Game Controller Register space
        
RESERVED            equ   $FF52  
          ; 158 bytes in reserve
        
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


