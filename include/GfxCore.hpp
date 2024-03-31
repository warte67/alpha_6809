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


    public:
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

    private:
        std::vector<GTIMING> vec_timings;
        std::vector<GMODE> vec_gmodes;
        void _init_gmodes();
};

/******

    Needed to track all of the graphics modes:

        Res_Width       // number of pixels per column
        Res_Height      // number of pixels per row
        Pixel_Width     // horizontal scan multiplier
        Pixel_Height    // vertical scan multiplier

        // calculated at run time?
        Timing_Width    // Res_Width * Pixel_Width
        Timing_Height   // Res_Height * Pixel_Height


 ******/