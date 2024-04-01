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

        // public accessors
        inline static void Present() { SDL_RenderPresent(sdl_renderer); }

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
        inline static SDL_Window* sdl_window = nullptr;
        inline static SDL_Renderer* sdl_renderer = nullptr;
        inline static SDL_Texture* sdl_target_texture = nullptr;
        Uint32 sdl_renderer_flags = 0;
        int window_width = 0;
        int window_height = 0;
        Uint32 window_flags = 0;
        bool bIsFullscreen = false;
        Word res_width = 0;
        Word res_height = 0;



        std::vector<GTIMING> vec_timings;
        std::vector<GMODE> vec_gmodes;   
        // helpers     
        void _init_gmodes();
        void _decode_gmode();
};

