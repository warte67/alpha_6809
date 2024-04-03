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

    private:

        // internal registers (do these really need to be statics?)
        inline static Byte s_gfx_mode   = 0x03;     // default: 3 = 320x200 text 
        inline static Byte s_gfx_emu    = 0x00;     // default: 0 = windowed... primary monitor
        inline static Byte _gfx_pal_idx = 0x00;     // GFX_PAL_IDX

        inline static Byte _gfx_glyph_idx = 0x00;         // GFX_GLYPH_IDX
        inline static Byte _gfx_glyph_data[256][8]{0};    // GFX_GLYPH_DATA (Customizeable)

        // helpers
        void _init_tests();

};


/******

   GFX_MODE     (Byte) Graphics Mode
                - bit 0-4   = Resolution Modes 0-31
                - bit 5-6   = 0:1bpp  1:2bpp  2:4bpp  3:8bpp
                - bit 7     = 0:text,  1:bitmap

    GFX_EMU     (Byte) Emulation Flags
                - bits 0-2  = Active Monitor 0-7
                - bits 3-5  = reserved
                - bit  6    = 0:vsync off, 1:vsync on
                - bit  7    = 0:windowed, 1:fullscreen

 ******/