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
    FIO_BUFFER       = 0x0100, 
    FIO_BFR_END      = 0x01FF, // End of the File Input/Output Buffer
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
        //           - bit 5-6   = Bits-Per-Pixel (in bitmap mode)
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
    CSR_PAL_INDX     = 0xFE27, //  (Byte) mouse cursor color palette index (0-15)
    CSR_PAL_DATA     = 0xFE28, //  (Word) mouse cursor color palette data RGBA4444
    CSR_END          = 0xFE2A, // End Mouse Registers
        
        // Keyboard Hardware Registers:
    KEY_BEGIN        = 0xFE2A, // Start of the Keyboard Register space
    CHAR_Q_LEN       = 0xFE2A, //   (Byte) # of characters waiting in queue        (Read Only)
    CHAR_SCAN        = 0xFE2B, //   (Byte) read next character in queue (not popped when read)
    CHAR_POP         = 0xFE2C, //   (Byte) read next character in queue (popped when read)
    XKEY_BUFFER      = 0xFE2D, //   (128 bits) 16 bytes for XK_KEY data buffer     (Read Only)
    EDT_BFR_CSR      = 0xFE3D, //   (Byte) cursor position within edit buffer     (Read/Write)
    EDT_ENABLE       = 0xFE3E, //   (Byte) line editor enable flag                 (Read/Write)
    EDT_BFR_LEN      = 0xFE3F, //   (Byte) Limit the line editor to this length   (Read/Write)
    EDT_BUFFER       = 0xFE40, //   line editing character buffer                 (Read Only)
    KEY_END          = 0xFF40, // End of the Keyboard Register space
        
    JOYS_BEGIN       = 0xFF40, // Start of the Game Controller Register space
    JOYS_1_BTN       = 0xFF40, //   (Word) button bits: room for up to 16 buttons  (realtime)
    JOYS_1_DBND      = 0xFF42, //   (Byte) PAD 1 analog deadband; default is 5   (read/write)
    JOYS_1_LTX       = 0xFF43, //   (char) PAD 1 LThumb-X position (-128 _ +127)   (realtime)
    JOYS_1_LTY       = 0xFF44, //   (char) PAD 1 LThumb-Y position (-128 _ +127)   (realtime)
    JOYS_1_RTX       = 0xFF45, //   (char) PAD 1 RThumb-X position (-128 _ +127)   (realtime)
    JOYS_1_RTY       = 0xFF46, //   (char) PAD 1 RThumb-Y position (-128 _ +127)   (realtime)
    JOYS_1_Z1        = 0xFF47, //   (char) PAD 1 left analog trigger (0 - 127)     (realtime)
    JOYS_1_Z2        = 0xFF48, //   (char) PAD 1 right analog trigger (0 - 127)    (realtime)
    JOYS_2_BTN       = 0xFF49, //   (Word) button bits: room for up to 16 buttons  (realtime)
    JOYS_2_DBND      = 0xFF4B, //   (Byte) PAD 2 analog deadband; default is 5   (read/write)
    JOYS_2_LTX       = 0xFF4C, //   (char) PAD 2 LThumb-X position (-128 _ +127)   (realtime)
    JOYS_2_LTY       = 0xFF4D, //   (char) PAD 2 LThumb-Y position (-128 _ +127)   (realtime)
    JOYS_2_RTX       = 0xFF4E, //   (char) PAD 2 RThumb-X position (-128 _ +127)   (realtime)
    JOYS_2_RTY       = 0xFF4F, //   (char) PAD 2 RThumb-Y position (-128 _ +127)   (realtime)
    JOYS_2_Z1        = 0xFF50, //   (char) PAD 2 left analog trigger (0 - 127)     (realtime)
    JOYS_2_Z2        = 0xFF51, //   (char) PAD 2 right analog trigger (0 - 127)    (realtime)
    JOYS_END         = 0xFF52, // End of the Game Controller Register space
        
    FIO_BEGIN        = 0xFF52, // Start of the FileIO register space
    FIO_ERR_FLAGS    = 0xFF52, // (Byte) File IO error flags
        // FIO_ERR_FLAGS: ABCD.EFGH
        //      A:  file was not found
        //      B:  directory was not found
        //      C:  file not open
        //      D:  end of file
        //      E:  buffer overrun
        //      F:  wrong file type
        //      G:  invalid command
        //      H:  incorrect file stream
        
    FIO_COMMAND      = 0xFF53, // (Byte) OnWrite, execute a file command (FC_<cmd>)
        // Begin FIO_COMMANDS
    FC_RESET         = 0x0000, //        Reset
    FC_SHUTDOWN      = 0x0001, //        SYSTEM: Shutdown
    FC_COMPDATE      = 0x0002, //        SYSTEM: Load Compilation Date
    FC_NEWFILE       = 0x0003, //      * New File Stream
    FC_OPENFILE      = 0x0004, //      * Open File
    FC_ISOPEN        = 0x0005, //      *Is File Open ? (returns FIO_ERR_FLAGS bit - 5)
    FC_CLOSEFILE     = 0x0006, //      * Close File
    FC_READBYTE      = 0x0007, //      * Read Byte (into FIO_IOBYTE)
    FC_WRITEBYTE     = 0x0008, //      * Write Byte (from FIO_IOBYTE)
    FC_LOADHEX       = 0x0009, //      * Load Hex Format File
    FC_GETLENGTH     = 0x000A, //        Get File Length (into FIO_IOWORD)
    FC_LISTDIR       = 0x000B, //        List Directory
    FC_MAKEDIR       = 0x000C, //      * Make Directory
    FC_CHANGEDIR     = 0x000D, //        Change Directory
    FC_GETPATH       = 0x000E, //        Fetch Current Path
    FC_REN_DIR       = 0x000F, //      * Rename Directory
    FC_DEL_DIR       = 0x0010, //      * Delete Directory
    FC_DEL_FILE      = 0x0011, //      * Delete File
    FC_REN_FILE      = 0x0012, //      * Rename file
    FC_COPYFILE      = 0x0013, //      * Copy File
    FC_SEEKSTART     = 0x0014, //      * Seek Start
    FC_SEEKEND       = 0x0015, //      * Seek End
    FC_SET_SEEK      = 0x0016, //      * Set Seek Position (from FIO_IOWORD)
    FC_GET_SEEK      = 0x0017, //      * Get Seek Position (into FIO_IOWORD)
        // End FIO_COMMANDS
        
    FIO_STREAM       = 0xFF54, // (Byte) current file stream index (0-15)
    FIO_MODE         = 0xFF55, // (Byte) Flags describing the I/O mode for the file
        // FIO_MODE: 00AB.CDEF  (indexed by FIO_STREAM)
        //      A:  INPUT - File open for reading
        //      B:  OUTPUT - File open for writing
        //      C:  BINARY - 1: Binary Mode, 0: Text Mode
        //      D:  AT_END - Output starts at the end of the file
        //      E:  APPEND - All output happens at end of the file
        //      F:  TRUNC - discard all previous file data
    FIO_SEEKPOS      = 0xFF56, // (DWord) file seek position
    FIO_IOBYTE       = 0xFF5A, // (Byte) input / output character
    FIO_IOWORD       = 0xFF5B, // (Byte) input / output character
    FIO_PATH_LEN     = 0xFF5C, // (Byte) length of the filepath
    FIO_PATH_POS     = 0xFF5D, // (Byte) character position within the filepath
    FIO_PATH_DATA    = 0xFF5E, // (Byte) data at the character position of the path
    FIO_DIR_DATA     = 0xFF5F, // (Byte) a series of null-terminated filenames
        //     NOTES: Current read-position is reset to the beginning following a 
        //             List Directory command. The read-position is automatically 
        //             advanced on read from this register. Each filename is 
        //             $0a-terminated. The list itself is null-terminated.
    FIO_END          = 0xFF60, // End of the FileIO register space
        
        // Math Co-Processor Hardware Registers:
    MATH_BEGIN       = 0xFF60, //  start of math co-processor  hardware registers
    MATH_ACA_POS     = 0xFF60, //  (Byte) character position within the ACA float string
    MATH_ACA_DATA    = 0xFF61, //  (Byte) ACA float string character port
    MATH_ACA_RAW     = 0xFF62, //  (4-Bytes) ACA raw float data
    MATH_ACA_INT     = 0xFF66, //  (4-Bytes) ACA integer data
    MATH_ACB_POS     = 0xFF6A, //  (Byte) character position within the ACB float string
    MATH_ACB_DATA    = 0xFF6B, //  (Byte) ACB float string character port
    MATH_ACB_RAW     = 0xFF6C, //  (4-Bytes) ACB raw float data
    MATH_ACB_INT     = 0xFF70, //  (4-Bytes) ACB integer data
    MATH_ACR_POS     = 0xFF74, //  (Byte) character position within the ACR float string
    MATH_ACR_DATA    = 0xFF75, //  (Byte) ACR float string character port
    MATH_ACR_RAW     = 0xFF76, //  (4-Bytes) ACR raw float data
    MATH_ACR_INT     = 0xFF7A, //  (4-Bytes) ACR integer data
    MATH_OPERATION   = 0xFF7E, //  (Byte) Operation 'command' to be issued
        // Begin MATH_OPERATION's (MOPS)
    MOP_RANDOM       = 0x0000, //        ACA, ACB, and ACR are set to randomized values
    MOP_RND_SEED     = 0x0001, //        MATH_ACA_INT seeds the pseudo-random number generator
    MOP_IS_EQUAL     = 0x0002, //        (bool)ACR = (ACA == ACB)
    MOP_IS_NOT_EQUAL = 0x0003, //        (bool)ACR = (ACA != ACB)
    MOP_IS_LESS      = 0x0004, //        (bool)ACR = std::isless(ACA, ACB);
    MOP_IS_GREATER   = 0x0005, //        (bool)ACR = std::isgreater(ACA, ACB);
    MOP_IS_LTE       = 0x0006, //        (bool)ACR = std::islessequal(ACA, ACB);
    MOP_IS_GTE       = 0x0007, //        (bool)ACR = std::islessgreater(ACA, ACB);
    MOP_IS_FINITE    = 0x0008, //        (bool)ACR = std::isfinite(ACA);
    MOP_IS_INF       = 0x0009, //        (bool)ACR = std::isinf(ACA);
    MOP_IS_NAN       = 0x000A, //        (bool)ACR = std::isnan(ACA);
    MOP_IS_NORMAL    = 0x000B, //        (bool)ACR = std::isnormal(ACA);
    MOP_SIGNBIT      = 0x000C, //        (bool)ACR = std::signbit(ACA);
    MOP_SUBTRACT     = 0x000D, //        ACR = ACA - ACB
    MOP_ADD          = 0x000E, //        ACR = ACA + ACB
    MOP_MULTIPLY     = 0x000F, //        ACR = ACA * ACB
    MOP_DIVIDE       = 0x0010, //        ACR = ACA / ACB
    MOP_FMOD         = 0x0011, //        ACR = std::fmod(ACA, ACB);
    MOP_REMAINDER    = 0x0012, //        ACR = std::remainder(ACA, ACB);
    MOP_FMAX         = 0x0013, //        ACR = std::fmax(ACA, ACB);
    MOP_FMIN         = 0x0014, //        ACR = std::fmin(ACA, ACB);
    MOP_FDIM         = 0x0015, //        ACR = std::fdim(ACA, ACB);
    MOP_EXP          = 0x0016, //        ACR = std::exp(ACA);
    MOP_EXP2         = 0x0017, //        ACR = std::exp2(ACA);
    MOP_EXPM1        = 0x0018, //        ACR = std::expm1(ACA);
    MOP_LOG          = 0x0019, //        ACR = std::log(ACA);
    MOP_LOG10        = 0x001A, //        ACR = std::log10(ACA);
    MOP_LOG2         = 0x001B, //        ACR = std::log2(ACA);
    MOP_LOG1P        = 0x001C, //        ACR = std::log1p(ACA);
    MOP_SQRT         = 0x001D, //        ACR = std::sqrt(ACA);
    MOP_CBRT         = 0x001E, //        ACR = std::cbrt(ACA);
    MOP_HYPOT        = 0x001F, //        ACR = std::hypot(ACA, ACB);
    MOP_POW          = 0x0020, //        ACR = std::pow(ACA, ACB);
    MOP_SIN          = 0x0021, //        ACR = std::sin(ACA);
    MOP_COS          = 0x0022, //        ACR = std::cos(ACA);
    MOP_TAN          = 0x0023, //        ACR = std::tan(ACA);
    MOP_ASIN         = 0x0024, //        ACR = std::asin(ACA);
    MOP_ACOS         = 0x0025, //        ACR = std::acos(ACA);
    MOP_ATAN         = 0x0026, //        ACR = std::atan(ACA);
    MOP_ATAN2        = 0x0027, //        ACR = std::atan2(ACA, ACB);
    MOP_SINH         = 0x0028, //        ACR = std::sinh(ACA);
    MOP_COSH         = 0x0029, //        ACR = std::acosh(ACA);
    MOP_ATANH        = 0x002A, //        ACR = std::atanh(ACA);
    MOP_ERF          = 0x002B, //        ACR = std::erf(ACA);
    MOP_ERFC         = 0x002C, //        ACR = std::erfc(ACA);
    MOP_LGAMMA       = 0x002D, //        ACR = std::lgamma(ACA);
    MOP_TGAMMA       = 0x002E, //        ACR = std::tgamma(ACA);
    MOP_CEIL         = 0x002F, //        ACR = std::ceil(ACA);
    MOP_FLOOR        = 0x0030, //        ACR = std::floor(ACA);
    MOP_TRUNC        = 0x0031, //        ACR = std::trunc(ACA);
    MOP_ROUND        = 0x0032, //        ACR = std::round(ACA);
    MOP_LROUND       = 0x0033, //        ACR = std::lround(ACA);
    MOP_NEARBYINT    = 0x0034, //        ACR = std::nearbyint(ACA);
    MOP_ILOGB        = 0x0035, //        ACR = std::ilogb(ACA);
    MOP_LOGB         = 0x0036, //        ACR = std::logb(ACA);
    MOP_NEXTAFTER    = 0x0037, //        ACR = std::nextafter(ACA, ACB);
    MOP_COPYSIGN     = 0x0038, //        ACR = std::copysign(ACA, ACB);
    MOP_LASTOP       = 0x0038, //        last implemented math operation 
        // End MATH_OPERATION's (MOPS)
    MATH_END         = 0xFF7F, // end of math co-processor registers
        
    RESERVED         = 0xFF7F, 
        // 113 bytes in reserve
        
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

