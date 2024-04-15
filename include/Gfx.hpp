// *************************************************
// *
// * Gfx.hpp
// *
// ***********************************
#pragma once

#include "IDevice.hpp" 

class Debug;

class Gfx : public IDevice
{
    friend class Debug; 
    friend class Mouse;
    friend class Memory;

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

        inline static Uint32 GetWindowID() { return SDL_GetWindowID(sdl_window); }
        inline static SDL_Window* GetSDLWindow() { return sdl_window; }

        Word GetTimingWidth();
        Word GetTimingHeight();


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
        void _updateExtendedBitmapScreen();
};



/**** NOTES ********************************************************************


    ToDo:
        1)  Add Load / Save palette files. 
        2)  Add Load / Save Glyph data (Character Fonts)
        3)  Add methods to copy palette data to/from memory address.
        4)  Add methods to copy font data to/from memory address.
        


 **** NOTES ********************************************************************/