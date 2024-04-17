// *************************************************
// *
// * Debug.hpp
// *
// ***********************************
#pragma once

// #include <map>
#include <unordered_map>
#include <list>
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
		void OnActivate() override;
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override;
		void OnUpdate(float fElapsedTime) override;
		void OnRender() override;

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        inline static Uint32 GetWindowID() { return SDL_GetWindowID(sdl_debug_window); }
        inline static SDL_Window* GetSDLWindow() { return sdl_debug_window; }
        inline static bool IsActive() { return s_bIsDebugActive; }

        bool IsCursorVisible() { return bIsCursorVisible; }

        bool SingleStep();
        void ContinueSingleStep();

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
        std::vector<PALETTE> _debug_palette;
	    Uint8 red(Uint8 index) { Uint8 c = _debug_palette[index].r;  return c; }
        Uint8 grn(Uint8 index) { Uint8 c = _debug_palette[index].g;  return c; }
        Uint8 blu(Uint8 index) { Uint8 c = _debug_palette[index].b;  return c; }
        Uint8 alf(Uint8 index) { Uint8 c = _debug_palette[index].a;  return c; }  


    private:



        // Word reg_brk_addr = 0x0000;	    // break point hardware register
        // Byte reg_flags = 0x00;			// debug flags hardware register

        enum CSR_AT {
            CSR_AT_NONE, CSR_AT_ADDRESS, CSR_AT_DATA, CSR_AT_REGISTER
        };
        enum EDIT_REGISTER {
            EDIT_NONE,
            EDIT_CC, EDIT_D, EDIT_A, EDIT_B, EDIT_X,
            EDIT_Y, EDIT_U, EDIT_PC, EDIT_S, EDIT_DP,
            EDIT_BREAK,
        };
        struct REGISTER_NODE {
            EDIT_REGISTER reg = EDIT_NONE;
            Uint16 value = 0;
            Uint16 y_pos = 0;
            Uint16 x_min = 0;
            Uint16 x_max = 0;
        };
        std::vector<REGISTER_NODE> register_info = {
            { EDIT_REGISTER::EDIT_NONE, 0, 0, 0, 0 },
            { EDIT_REGISTER::EDIT_CC, 0, 1, 43, 44 },
            { EDIT_REGISTER::EDIT_D, 0, 2, 42, 45 },
            { EDIT_REGISTER::EDIT_A, 0, 2, 51, 52 },
            { EDIT_REGISTER::EDIT_B, 0, 2, 57, 58 },
            { EDIT_REGISTER::EDIT_X, 0, 3, 43, 46 },
            { EDIT_REGISTER::EDIT_Y, 0, 3, 51, 54 },
            { EDIT_REGISTER::EDIT_U, 0, 3, 59, 62 },
            { EDIT_REGISTER::EDIT_PC, 0, 4, 43, 46 },
            { EDIT_REGISTER::EDIT_S, 0, 4, 51, 54 },
            { EDIT_REGISTER::EDIT_DP, 0, 4, 60, 61 },
            { EDIT_REGISTER::EDIT_BREAK, 0, 33, 50, 53 },
        };
        REGISTER_NODE nRegisterBeingEdited = { EDIT_NONE,0,0,0,0 };

        struct BUTTON_NODE {
            std::string text;		// button text
            SDL_Scancode key;		// shortcut key scancode
            // color attribute
            // ...
            Uint16 x_min;			// button left
            Uint16 x_max;			// button right
            Uint16 y_pos;
            Byte clr_index;			// color index
            void (Debug::* cbFunction)();	// button callback
        };
        std::vector<BUTTON_NODE> vButton = {	
            {" Clr Breaks",		SDL_SCANCODE_C,		51, 62, 31, 0xC, &Debug::cbClearBreaks},
            {"Reset",			SDL_SCANCODE_R,		43, 49, 31, 0xB, &Debug::cbReset },
            {"NMI",				SDL_SCANCODE_N,		37, 41, 31, 0xD, &Debug::cbNMI },
            {"IRQ",				SDL_SCANCODE_I,		31, 35, 31, 0xD, &Debug::cbIRQ },
            {" FIRQ",			SDL_SCANCODE_F,		24, 29, 31, 0xD, &Debug::cbFIRQ },
            {" RUN",			SDL_SCANCODE_D,		17, 22, 31, 0xB, &Debug::cbRunStop },
            {" EXIT",			SDL_SCANCODE_H,		17, 22, 33, 0xB, &Debug::cbHide },
            {"STEP_INTO",		SDL_SCANCODE_SPACE,	24, 34, 33, 0x9, &Debug::cbStepIn },
            {"STEP_OVER",		SDL_SCANCODE_O,		36, 46, 33, 0x9, &Debug::cbStepOver },
            {"ADD BRK",			SDL_SCANCODE_B,		48, 54, 33, 0xC, &Debug::cbAddBrk },
        };

        // button callbacks
        void cbClearBreaks();
        void cbReset();
        void cbNMI();
        void cbIRQ();
        void cbFIRQ();
        void cbRunStop();
        void cbHide();
        void cbStepIn();
        void cbStepOver();
        void cbAddBrk();

        std::vector <Word> mem_bank = { SSTACK_TOP - 0x0048, VIDEO_START, GFX_MODE };
        std::vector <Word> sDisplayedAsm;
        std::unordered_map<Word, bool> mapBreakpoints;	
        std::list<Word> asmHistory;		// track last several asm addresses

        int csr_x = 0;
        int csr_y = 0;
        int csr_at = CSR_AT::CSR_AT_NONE;
        char mouse_wheel = 0;
        bool bIsStepPaused = true;
        bool bIsCursorVisible = false;
        Word mousewheel_offset = 0;		// applies to code scroll
        bool bMouseWheelActive = false; // applies to code scroll
        int mw_brk_offset = 0;			// mouse wheel adjusts the offset
        bool bIsMouseOver = false;

        bool bEditingBreakpoint = false;
        Word new_breakpoint = 0;		// working copy to be edited
        Word reg_brk_addr = 0x0000;	    // break point hardware register
        Byte reg_flags = 0x00;			// debug flags hardware register

        inline static SDL_Window* sdl_debug_window = nullptr;
        inline static SDL_Renderer* sdl_debug_renderer = nullptr;
        inline static SDL_Texture* sdl_debug_target_texture = nullptr;

        inline static bool s_bIsDebugActive = DEBUG_STARTS_ACTIVE;
        inline static bool s_bSingleStep = DEBUG_SINGLE_STEP;
        inline static bool s_bIsStepPaused = true;        


        Uint32 sdl_debug_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
        Uint32 debug_window_flags = SDL_WINDOW_RESIZABLE;

 		void _setPixel(int x, int y, Byte color_index, 	
						SDL_Texture* _texture, bool bIgnoreAlpha = false);
        void _setPixel_unlocked(void* pixels, int pitch, int x, int y, 
								Byte color_index, bool bIgnoreAlpha = false);      
        void _updateDebugScreen();
        void _correctMouseCoords(int& mx, int& my);

        void OutGlyph(int col, int row, Byte glyph, Byte color_index);
        int OutText(int col, int row, std::string text, Byte color_index);        

        bool CoordIsValid(int x, int y);
        void MouseStuff();
        void KeyboardStuff();
        std::string _hex(Uint32 n, Uint8 d);
        void DumpMemory(int col, int row, Word addr);
        void DrawCpu(int x, int y);
        void DrawCode(int col, int row);
        void DrawButtons();
        void HandleButtons();
        void DrawBreakpoints();
        bool EditRegister(float fElapsedTime);
        void DrawCursor(float fElapsedTime);


        struct D_GLYPH 
        { 
            Byte chr; 
            Byte attr;
        };
        std::vector<D_GLYPH> _db_bfr{0};

};
