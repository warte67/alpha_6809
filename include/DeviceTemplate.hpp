// *************************************************
// *
// * DeviceTemplate.hpp
// *
// ***********************************
#pragma once

#include "IDevice.hpp" 

class DeviceTemplate : public IDevice
{
    public:
        DeviceTemplate() { _deviceName = "DeviceTemplate"; }
        DeviceTemplate(std::string sName) : IDevice(sName) {}
        ~DeviceTemplate() {};

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
};
