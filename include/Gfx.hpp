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

    private:

        // internal registers
        inline static Byte s_gfx_mode = 03;    // default: 3= 320x200 text 

};


/******

   GFX_MODE     (Byte) Graphics Mode
                - bit 0-4   = Resolution Modes 0-31
                - bit 5     = 0:windowed, 1:fullscreen (emulation only)
                - bit 6     = 0:vsync off, 1:vsync on
                - bit 7     = 0:text,  1:bitmap

 ******/