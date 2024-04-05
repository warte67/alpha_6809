// *************************************************
// *
// * Mouse.hpp
// *
// ***********************************
#pragma once

#include "IDevice.hpp" 

class Mouse : public IDevice
{
    public:
        Mouse() { _deviceName = "Mouse"; }
        Mouse(std::string sName) : IDevice(sName) {}
        ~Mouse() {};

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

        std::vector<std::string> csr_buffer = 
        {
            "0000            ",
            "08f0000         ",
            "018fff00000     ",
            " 018ffffff0     ",
            " 0118fff000     ",
            " 01118000       ",
            " 001100         ",
            "  0110          ",
            "  0100          ",
            "  010           ",
            "  000           ",
            "                ",
            "                ",
            "                ",
            "                ",
            "                "
        };

        SDL_Texture* _cursor_texture = nullptr;

        int mouse_x = 0;			// horizontal mouse cursor position
        int mouse_y = 0;			// vertical mouse cursor position
        char mouse_wheel = 0;		// signed mouse wheel delta
        Uint8 mouse_x_offset = 0;	// mouse cursor offset x
        Uint8 mouse_y_offset = 0;	// mouse cursor offset y
        Byte button_flags = 0;		// bits 0-4: button states, bit 5: cursor enable, bits 6-7: number of clicks
        bool _bCsrIsDirty = true;		// internal flag
        // bool _bUseSdlCursor = ENABLE_SDL_MOUSE_CURSOR;  // was _bCsrIsVisible

        // cursor stuff
        Byte bmp_offset = 0;
        Byte cursor_buffer[16][16];     // indexed 16-color bitmap data for the mouse cursor


        // private helpers
        void _show_SDL_cursor(bool b);      // hides or shows the SDL and Hardware cursors 
        void _display_SDL_cursor();

};
