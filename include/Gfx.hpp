// *************************************************
// *
// * Gfx.hpp
// *
// ***********************************
#pragma once

#include "IDevice.hpp" 

class Gfx : public IDevice
{
    public:
        Gfx() { _deviceName = "Gfx"; }
        Gfx(std::string sName) : IDevice(sName) {}
        ~Gfx() {};

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnEvent(SDL_Event* evnt) override;
		void OnUpdate(float fElapsedTime) override;
		void OnRender() override;

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        struct GTIMING
        {
            Word Width;         // display width
            Word Height;        // display height
        };
        struct GMODE 
        {            
            Byte Timing_index;  // 
            Word Res_Width;     // number of pixels per column
            Word Res_Height;    // number of pixels per row
            Byte Pixel_Width;   // horizontal scan multiplier
            Byte Pixel_Height;  // vertical scan multiplier           
            // ... one byte wasted 
        };

		// palette stuff
        union PALETTE {
            Word color;
            struct {
                Uint8 b : 4;
                Uint8 g : 4;
                Uint8 r : 4;
                Uint8 a : 4;
            };
        };
        std::vector<PALETTE> _palette;
	    Uint8 red(Uint8 index) { Uint8 c = _palette[index].r;  return c; }
        Uint8 grn(Uint8 index) { Uint8 c = _palette[index].g;  return c; }
        Uint8 blu(Uint8 index) { Uint8 c = _palette[index].b;  return c; }
        Uint8 alf(Uint8 index) { Uint8 c = _palette[index].a;  return c; }  

        // public accessors
        inline static Byte GetGlyphData(Byte index, Byte row) { return _gfx_glyph_data[index][row]; }
        inline static void Present() { SDL_RenderPresent(sdl_renderer); }
        bool VerifyGmode(Byte gmode);



    private:

        // internal registers (do these really need to be statics?)
        inline static Byte s_gfx_mode   = 0x0E;     // defaults: 0x03 = 40x25 text   0x0E = 32x15 text (16:9)
        inline static Byte s_gfx_emu    = 0x00;     // default: 0 = windowed... primary monitor
        inline static Byte _gfx_pal_idx = 0x00;     // GFX_PAL_IDX
        inline static Byte _gfx_glyph_idx = 0x00;         // GFX_GLYPH_IDX
        inline static Byte _gfx_glyph_data[256][8]{0};    // GFX_GLYPH_DATA (Customizeable)
        inline static SDL_Window* sdl_window = nullptr;
        inline static SDL_Renderer* sdl_renderer = nullptr;
        inline static SDL_Texture* sdl_target_texture = nullptr;
        Word gfx_vid_end = VIDEO_END;

        Uint32 sdl_renderer_flags = 0;
        int window_width = 0;
        int window_height = 0;
        Uint32 window_flags = 0;
        bool bIsFullscreen = false;
        bool bIsBitmapMode = false;
        Word res_width = 0;
        Word res_height = 0;
        Byte bits_per_pixel = 1;

        std::vector<GTIMING> vec_timings;
        std::vector<GMODE> vec_gmodes;   

        // helpers
        void _init_tests();
        void _init_gmodes();
        void _decode_gmode();
 		void _setPixel(int x, int y, Byte color_index, 	
						SDL_Texture* _texture, bool bIgnoreAlpha = false);
        void _setPixel_unlocked(void* pixels, int pitch, int x, int y, 
								Byte color_index, bool bIgnoreAlpha = false);        
        void _updateTextScreen();        
        void _updateBitmapScreen();    
};


/******
              
    GFX_MODE         = 0xFE00, // (Byte) Graphics Mode
        //           - bit 0-4   = Resolution Modes 0-31
        //           - bit 7     = 0:text,  1:bitmap
        
    GFX_EMU          = 0xFE01, // (Byte) Emulation Flags
        //           - bits 0-2  = Active Monitor 0-7
        //           - bits 3-5  = reserved
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
        
    GFX_PAL_IDX      = 0xFE02, //  (Byte) Color Palette Index
        // GFX_PAL_IDX: 0-255
        // Note: Use this register to set the index into theColor Palette. 
        //   Set this value prior referencing color data (GFX_PAL_CLR).
        
    GFX_PAL_CLR      = 0xFE03, //  (Word) Indexed Color Palette Data
        // GFX_PAL_CLR: Color Data A4R4G4B4 format
        // Note: This is the color data for an individual palette entry.
        //     Write to DSP_PAL_IDX with the index within the color palette
        //     prior to reading or writing the color data in the GFX_PAL_CLR register.
        
    GFX_GLYPH_IDX    = 0xFE05, //  (Byte) Text Glyph Index
        // GFX_GLYPH_IDX: 0-256
        // Note: Set this register to index a specific text glyph. Set this value
        //     prior to updating glyph pixel data.
        
    GFX_GLYPH_DATA   = 0xFE06, //  (8-Bytes) Text Glyph Pixel Data Array
        // GFX_GLYPH_DATA: 8 rows of binary encoded glyph pixel data
        // Note: Each 8x8 text glyph is composed of 8 bytes. The first byte in this 
        //     array represents the top line of 8 pixels. Each array entry represents
        //     a row of 8 pixels.                 

 ******/