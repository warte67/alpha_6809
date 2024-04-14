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
        
          ; Banked Memory Buffers (16K total):
MEMBANK_BEGIN       equ   $B000    ;  start of Banked Memory Region
MEMBANK_ONE         equ   $B000    ;  Start of Page One (8K)
MEMBANK_TWO         equ   $D000    ;  Start of Page Two (8K)
MEMBANK_END         equ   $F000    ;  End of Banked Memory Region
        
          ; KERNEL ROM (3.5K)
KERNEL_ROM          equ   $F000  
        
          ; HARDWARE REGISTERS (0.5K)
GFX_BEGIN           equ   $FE00    ;  Start of Graphics Hardware Registers
GFX_MODE            equ   $FE00    ; (Byte) Graphics Mode
          ;          - bit 0-4   = Resolution Modes 0-31
          ;          - bit 5-6   = Bits-Per-Pixel (in bitmap mode)
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
        
FIO_BEGIN           equ   $FF52    ; Start of the FileIO register space
FIO_ERROR           equ   $FF52    ; (Byte) FILE_ERROR enumeration result
          ; Begin FILE_ERROR enumeration
FE_NOERROR          equ   $0000    ;      $00: no error, condition normal
FE_NOTFOUND         equ   $0001    ;      $01: file or folder not found
FE_NOTOPEN          equ   $0002    ;      $02: file not open
FE_EOF              equ   $0003    ;      $03: end of file
FE_OVERRUN          equ   $0004    ;      $04: buffer overrun
FE_WRONGTYPE        equ   $0005    ;      $05: wrong file type
FE_BAD_CMD          equ   $0006    ;      $06: invalid command
FE_BADSTREAM        equ   $0007    ;      $07: invalid file stream
          ; End FILE_ERROR enumeration
        
FIO_COMMAND         equ   $FF53    ; (Byte) OnWrite, execute a file command (FC_<cmd>)
          ; Begin FIO_COMMANDS
FC_RESET            equ   $0000    ;        Reset
FC_SHUTDOWN         equ   $0001    ;        SYSTEM: Shutdown
FC_COMPDATE         equ   $0002    ;        SYSTEM: Load Compilation Date
FC_FILEEXISTS       equ   $0003    ;        Does File Exist (return in FIO_IODATA)
FC_OPENREAD         equ   $0004    ;      * Open Binary File For Reading
FC_OPENWRITE        equ   $0005    ;      * Open Binary File For Writing
FC_OPENAPPEND       equ   $0006    ;      * Open Binary File For Appending
FC_CLOSEFILE        equ   $0007    ;      * Close File
FC_READBYTE         equ   $0008    ;      * Read Byte (into FIO_IOBYTE)
FC_WRITEBYTE        equ   $0009    ;      * Write Byte (from FIO_IOBYTE)
FC_LOADHEX          equ   $000A    ;        Load Hex Format File
FC_GETLENGTH        equ   $000B    ;        Get File Length (into FIO_IOWORD)
FC_LISTDIR          equ   $000C    ;        List Directory
FC_MAKEDIR          equ   $000D    ;      * Make Directory
FC_CHANGEDIR        equ   $000E    ;        Change Directory
FC_GETPATH          equ   $000F    ;        Fetch Current Path
FC_REN_DIR          equ   $0010    ;      * Rename Directory
FC_DEL_DIR          equ   $0011    ;      * Delete Directory
FC_DEL_FILE         equ   $0012    ;      * Delete File
FC_REN_FILE         equ   $0013    ;      * Rename file
FC_COPYFILE         equ   $0014    ;      * Copy File
FC_SEEKSTART        equ   $0015    ;      * Seek Start
FC_SEEKEND          equ   $0016    ;      * Seek End
FC_SET_SEEK         equ   $0017    ;      * Set Seek Position (from FIO_IOWORD)
FC_GET_SEEK         equ   $0018    ;      * Get Seek Position (into FIO_IOWORD)
          ; End FIO_COMMANDS
        
FIO_HANDLE          equ   $FF54    ; (Byte) current file stream HANDLE 0=NONE
FIO_SEEKPOS         equ   $FF55    ; (DWord) file seek position
FIO_IODATA          equ   $FF59    ; (Byte) input / output character
FIO_PATH_LEN        equ   $FF5A    ; (Byte) length of the filepath
FIO_PATH_POS        equ   $FF5B    ; (Byte) character position within the filepath
FIO_PATH_DATA       equ   $FF5C    ; (Byte) data at the character position of the path
FIO_DIR_DATA        equ   $FF5D    ; (Byte) a series of null-terminated filenames
          ;     NOTES: Current read-position is reset to the beginning following a 
          ;             List Directory command. The read-position is automatically 
          ;             advanced on read from this register. Each filename is 
          ;             $0a-terminated. The list itself is null-terminated.
FIO_END             equ   $FF5E    ; End of the FileIO register space
        
          ; Math Co-Processor Hardware Registers:
MATH_BEGIN          equ   $FF5E    ;  start of math co-processor  hardware registers
MATH_ACA_POS        equ   $FF5E    ;  (Byte) character position within the ACA float string
MATH_ACA_DATA       equ   $FF5F    ;  (Byte) ACA float string character port
MATH_ACA_RAW        equ   $FF60    ;  (4-Bytes) ACA raw float data
MATH_ACA_INT        equ   $FF64    ;  (4-Bytes) ACA integer data
MATH_ACB_POS        equ   $FF68    ;  (Byte) character position within the ACB float string
MATH_ACB_DATA       equ   $FF69    ;  (Byte) ACB float string character port
MATH_ACB_RAW        equ   $FF6A    ;  (4-Bytes) ACB raw float data
MATH_ACB_INT        equ   $FF6E    ;  (4-Bytes) ACB integer data
MATH_ACR_POS        equ   $FF72    ;  (Byte) character position within the ACR float string
MATH_ACR_DATA       equ   $FF73    ;  (Byte) ACR float string character port
MATH_ACR_RAW        equ   $FF74    ;  (4-Bytes) ACR raw float data
MATH_ACR_INT        equ   $FF78    ;  (4-Bytes) ACR integer data
MATH_OPERATION      equ   $FF7C    ;  (Byte) Operation 'command' to be issued
          ; Begin MATH_OPERATION's (MOPS)
MOP_RANDOM          equ   $0000    ;        ACA, ACB, and ACR are set to randomized values
MOP_RND_SEED        equ   $0001    ;        MATH_ACA_INT seeds the pseudo-random number generator
MOP_IS_EQUAL        equ   $0002    ;        (bool)ACR = (ACA == ACB)
MOP_IS_NOT_EQUAL    equ   $0003    ;        (bool)ACR = (ACA != ACB)
MOP_IS_LESS         equ   $0004    ;        (bool)ACR = std::isless(ACA, ACB);
MOP_IS_GREATER      equ   $0005    ;        (bool)ACR = std::isgreater(ACA, ACB);
MOP_IS_LTE          equ   $0006    ;        (bool)ACR = std::islessequal(ACA, ACB);
MOP_IS_GTE          equ   $0007    ;        (bool)ACR = std::islessgreater(ACA, ACB);
MOP_IS_FINITE       equ   $0008    ;        (bool)ACR = std::isfinite(ACA);
MOP_IS_INF          equ   $0009    ;        (bool)ACR = std::isinf(ACA);
MOP_IS_NAN          equ   $000A    ;        (bool)ACR = std::isnan(ACA);
MOP_IS_NORMAL       equ   $000B    ;        (bool)ACR = std::isnormal(ACA);
MOP_SIGNBIT         equ   $000C    ;        (bool)ACR = std::signbit(ACA);
MOP_SUBTRACT        equ   $000D    ;        ACR = ACA - ACB
MOP_ADD             equ   $000E    ;        ACR = ACA + ACB
MOP_MULTIPLY        equ   $000F    ;        ACR = ACA * ACB
MOP_DIVIDE          equ   $0010    ;        ACR = ACA / ACB
MOP_FMOD            equ   $0011    ;        ACR = std::fmod(ACA, ACB);
MOP_REMAINDER       equ   $0012    ;        ACR = std::remainder(ACA, ACB);
MOP_FMAX            equ   $0013    ;        ACR = std::fmax(ACA, ACB);
MOP_FMIN            equ   $0014    ;        ACR = std::fmin(ACA, ACB);
MOP_FDIM            equ   $0015    ;        ACR = std::fdim(ACA, ACB);
MOP_EXP             equ   $0016    ;        ACR = std::exp(ACA);
MOP_EXP2            equ   $0017    ;        ACR = std::exp2(ACA);
MOP_EXPM1           equ   $0018    ;        ACR = std::expm1(ACA);
MOP_LOG             equ   $0019    ;        ACR = std::log(ACA);
MOP_LOG10           equ   $001A    ;        ACR = std::log10(ACA);
MOP_LOG2            equ   $001B    ;        ACR = std::log2(ACA);
MOP_LOG1P           equ   $001C    ;        ACR = std::log1p(ACA);
MOP_SQRT            equ   $001D    ;        ACR = std::sqrt(ACA);
MOP_CBRT            equ   $001E    ;        ACR = std::cbrt(ACA);
MOP_HYPOT           equ   $001F    ;        ACR = std::hypot(ACA, ACB);
MOP_POW             equ   $0020    ;        ACR = std::pow(ACA, ACB);
MOP_SIN             equ   $0021    ;        ACR = std::sin(ACA);
MOP_COS             equ   $0022    ;        ACR = std::cos(ACA);
MOP_TAN             equ   $0023    ;        ACR = std::tan(ACA);
MOP_ASIN            equ   $0024    ;        ACR = std::asin(ACA);
MOP_ACOS            equ   $0025    ;        ACR = std::acos(ACA);
MOP_ATAN            equ   $0026    ;        ACR = std::atan(ACA);
MOP_ATAN2           equ   $0027    ;        ACR = std::atan2(ACA, ACB);
MOP_SINH            equ   $0028    ;        ACR = std::sinh(ACA);
MOP_COSH            equ   $0029    ;        ACR = std::acosh(ACA);
MOP_ATANH           equ   $002A    ;        ACR = std::atanh(ACA);
MOP_ERF             equ   $002B    ;        ACR = std::erf(ACA);
MOP_ERFC            equ   $002C    ;        ACR = std::erfc(ACA);
MOP_LGAMMA          equ   $002D    ;        ACR = std::lgamma(ACA);
MOP_TGAMMA          equ   $002E    ;        ACR = std::tgamma(ACA);
MOP_CEIL            equ   $002F    ;        ACR = std::ceil(ACA);
MOP_FLOOR           equ   $0030    ;        ACR = std::floor(ACA);
MOP_TRUNC           equ   $0031    ;        ACR = std::trunc(ACA);
MOP_ROUND           equ   $0032    ;        ACR = std::round(ACA);
MOP_LROUND          equ   $0033    ;        ACR = std::lround(ACA);
MOP_NEARBYINT       equ   $0034    ;        ACR = std::nearbyint(ACA);
MOP_ILOGB           equ   $0035    ;        ACR = std::ilogb(ACA);
MOP_LOGB            equ   $0036    ;        ACR = std::logb(ACA);
MOP_NEXTAFTER       equ   $0037    ;        ACR = std::nextafter(ACA, ACB);
MOP_COPYSIGN        equ   $0038    ;        ACR = std::copysign(ACA, ACB);
MOP_LASTOP          equ   $0038    ;        last implemented math operation 
          ; End MATH_OPERATION's (MOPS)
MATH_END            equ   $FF7D    ; end of math co-processor registers
        
          ; Memory Device Hardware Registers
MEM_BEGIN           equ   $FF7D    ;  Start of Memory Device Hardware Registers
MEM_BANK1_SELECT    equ   $FF7E    ; (Byte) select 8k page for bank 1 (0-255)
MEM_BANK2_SELECT    equ   $FF7F    ; (Byte) select 8k page for bank 2 (0-255)
MEM_BANK1_TYPE      equ   $FF80    ; (Byte) memory bank 1 type
MEM_BANK2_TYPE      equ   $FF81    ; (Byte) memory bank 2 type
MEM_TYPE_RAM        equ   $0000    ;      random access memory (RAM)
MEM_TYPE_PERSIST    equ   $0001    ;      persistent memory (saved RAM)
MEM_TYPE_ROM        equ   $0002    ;      read only memory (ROM)
MEM_EXT_ADDR        equ   $FF82    ; (Word) Extended Memory Address Port
MEM_EXT_PITCH       equ   $FF84    ; (Word) number of bytes per line
MEM_EXT_WIDTH       equ   $FF86    ; (Word) width before skipping to next line
MEM_EXT_DATA        equ   $FF88    ; (Byte) External Memory Data Port
MEM_END             equ   $FF89    ;  End of Memory Device Hardware Registers
        
          ; Reserved for Future Hardware Devices
RSRVD_DEVICE_MEM    equ   $FF89  
          ; 103 bytes in reserve
        
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
