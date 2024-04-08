// *************************************************
// *
// * Math.hpp
// *
// ***********************************
#pragma once

#include "IDevice.hpp" 

class Math : public IDevice
{
    public:
        Math() { _deviceName = "Math"; }
        Math(std::string sName) : IDevice(sName) {}
        ~Math() {};

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override {}
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override {}
		void OnUpdate(float fElapsedTime) override {}
		void OnRender() override {}

        // virtuals
        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;

    private:
        // ACA:  Float Accumilator A
        float aca_float = 0.0f;		// the ACA working float
        Byte aca_pos = 0;			// character position within ACA string
        std::string aca_string = "";	// internal ACA float string
        DWord aca_raw = 0;			// internal ACA raw float data
        DWord aca_int = 0;			// internal ACA integer data

        // ACB:  Float Accumilator B
        float acb_float = 0.0f;		// the ACB working float
        Byte acb_pos = 0;			// character position within ACB string
        std::string acb_string = "";	// internal ACB float string
        DWord acb_raw = 0;			// internal ACB raw float data
        DWord acb_int = 0;			// internal ACB integer data

        // ACR:  Float Accumilator R (return)
        float acr_float = 0.0f;		// the ACR working float
        Byte acr_pos = 0;			// character position within ACR string
        std::string acr_string = "";	// internal ACR float string
        DWord acr_raw = 0;			// internal ACR raw float data
        DWord acr_int = 0;			// internal ACR integer data

        // MATH_OPERATION
        Byte math_operation = 0;	// Operation 'command' 
        DWord math_random_seed = 0;	// random seed

        // HELPERS
        Byte _read_acc(Word offset, Word reg, Byte& _pos, 
                float& _float, std::string& _string, DWord& _raw, DWord& _int);
        Byte _write_acc(Word offset, Byte data, Word reg, Byte& _pos, 
                float& _float, std::string& _string, DWord& _raw, DWord& _int);
        void _update_regs_from_float(float f_data, float& _float,
                Byte& _pos, std::string& _string, DWord& _raw, DWord& _int);
};



