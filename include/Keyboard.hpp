/**** Keyboard.hpp ***************************************
 *
 *   Acts to emulate a Raspberry PI Pico #2
 *		Dispatches Keyboard Events to PICO #1
 *		Emulates the Keyboard Hardware Registers
 *
 *  Lisenced under GPL version 3
 ************************************/
#pragma once

#include <unordered_map>
#include <queue>
#include <array>

#include "IDevice.hpp" 
#include "XK_Keycodes.hpp"


// This should never be increased. There is an 8-bit math limitation imposed on the 6809.
constexpr size_t EDIT_BUFFER_SIZE = 256;        // if this is changed, the memory_map.hpp must be rebuilt


class Keyboard : public IDevice
{
    public:
        Keyboard() { _deviceName = "Keyboard"; }
        Keyboard(std::string sName) : IDevice(sName) {}
        ~Keyboard() {};

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnEvent(SDL_Event* evnt) override;

        // unused:
		void OnQuit() override {};
		void OnActivate() override {};
		void OnDeactivate() override {};
		void OnUpdate(float fElapsedTime) override {};
		void OnRender() override {};

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

        // public member functions
        int charQueueLen();
        Byte charScanQueue();
        Byte charPopQueue();
        XKey::XK TranslateSDLtoXKey(SDL_Scancode k);
        XKey::XK AscToXKey(Byte asc);
        char XKeyToAscii(XKey::XK xKey);
        bool IsKeyDown(Byte xk);       

private:

	std::unordered_map<XKey::XK, int> keyMap;
	std::vector<std::tuple<XKey::XK, char, char>> xkToAsciiMap;
	std::queue <Byte> charQueue;		// ascii character queue
	std::array <Byte, EDIT_BUFFER_SIZE> editBuffer;	// internal line edit character buffer
	Byte edt_bfr_csr = 0;				// internal edit character buffer cursor position
	std::string _str_edt_buffer = "";
	bool _line_editor_enable = false;
    Byte _line_editor_length = (Byte)EDIT_BUFFER_SIZE-1;

    // helpers
	void _doEditBuffer(char  xkey);         
};


