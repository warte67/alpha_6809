// *************************************************
// *
// * GfxVRam.hpp
// *
// *        Handles the standard video memory
// *
// ***********************************
#pragma once

#include "IDevice.hpp"

class GfxVRam : public IDevice
{
    public:
        GfxVRam() { _deviceName = "GfxVRam"; }
        GfxVRam(std::string sName) : IDevice(sName) {}
        ~GfxVRam() {};

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
