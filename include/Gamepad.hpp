// *************************************************
// *
// * Gamepad.hpp
// *
// ***********************************
#pragma once

#include <unordered_map>
#include "IDevice.hpp" 

class Gamepad : public IDevice
{
    public:
        Gamepad() { _deviceName = "Gamepad"; }
        Gamepad(std::string sName) : IDevice(sName) {}
        ~Gamepad() {};

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnEvent(SDL_Event* evnt) override;
		void OnUpdate(float fElapsedTime) override;

        // not used
		void OnRender() override {};

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

    private:

        void OpenControllers();
        void CloseControllers();
        void InitButtonStates();
        Word EncodeButtonRegister(int id);
        void EncodeAxesRegister(int id);

        struct STATE {
            SDL_GameController* controller = nullptr;
            SDL_Joystick* joystick = nullptr;
            bool bIsActive = false;
            bool bIsGamepad = false;
            std::string name = "";
        };

        enum GPAD_BTN {
            BTN_A,
            BTN_B,
            BTN_X,
            BTN_Y,
            BTN_BACK,
            BTN_START,
            BTN_LS,
            BTN_RS,
            BTN_DPAD_UP,
            BTN_DPAD_DOWN,
            BTN_DPAD_LEFT,
            BTN_DPAD_RIGHT,
            BTN_GUIDE,
        };

        enum JOYS_BTN {
            BTN_1,
            BTN_2,
            BTN_3,
            BTN_4,
            BTN_5,
            BTN_6,
            BTN_7,
            BTN_8,
            BTN_9,
            BTN_10,
            BTN_11,
            BTN_12,
            BTN_HAT_UP,
            BTN_HAT_DOWN,
            BTN_HAT_LEFT,
            BTN_HAT_RIGHT,
        };

        bool bGamepadWasInit = false;
        STATE state[2];
        std::unordered_map<Word, Byte> joysBtnMap;
        std::unordered_map<Word, Byte> gpadBtnMap;    
};
