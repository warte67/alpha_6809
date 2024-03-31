// *************************************************
// *
// * GfxCore.hpp
// *
// *    1) Handles the standard video memory.
// *    2) Maintains the SDL window, renderer, and main display buffer texture
// *    3) Contains the potential display mode scan timing data
// *
// ***********************************
#pragma once

#include "IDevice.hpp"

class GfxCore : public IDevice
{
    public:
        GfxCore() { _deviceName = "GfxCore"; }
        GfxCore(std::string sName) : IDevice(sName) {}
        ~GfxCore() {};

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
        SDL_Window* sdl_window = nullptr;
};
