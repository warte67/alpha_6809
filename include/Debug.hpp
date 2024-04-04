// *************************************************
// *
// * Debug.hpp
// *
// ***********************************
#pragma once

#include <map>
#include <unordered_map>
#include "IDevice.hpp" 

class Debug : public IDevice
{
    public:
        Debug() { _deviceName = "Debug"; }
        Debug(std::string sName) : IDevice(sName) {}
        ~Debug() {};

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override;
		void OnUpdate(float fElapsedTime) override;
		void OnRender() override;

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        inline static Uint32 GetWindowID() { return SDL_GetWindowID(sdl_debug_window); }
        inline static bool IsActive() { return s_bIsDebugActive; }

        bool SingleStep();
        void ContinueSingleStep();


    private:

        inline static SDL_Window* sdl_debug_window = nullptr;
        inline static SDL_Renderer* sdl_debug_renderer = nullptr;
        inline static SDL_Texture* sdl_debug_target_texture = nullptr;

        inline static bool s_bIsDebugActive = false;
        inline static bool s_bSingleStep = DEBUG_SINGLE_STEP;
        inline static bool s_bIsStepPaused = true;


        Uint32 sdl_debug_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
        Uint32 debug_window_flags = SDL_WINDOW_RESIZABLE;

 		void _setPixel(int x, int y, Byte color_index, 	
						SDL_Texture* _texture, bool bIgnoreAlpha = false);
        void _setPixel_unlocked(void* pixels, int pitch, int x, int y, 
								Byte color_index, bool bIgnoreAlpha = false);      
        void _updateDebugScreen();

        struct D_GLYPH 
        { 
            Byte chr; 
            Byte attr;
        };
        std::vector<D_GLYPH> _db_bfr{0};


      	// std::map<Word, bool> mapBreakpoints;	
      	std::unordered_map<Word, bool> mapBreakpoints;	

};
