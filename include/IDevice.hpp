// IDevice.hpp
//
#pragma once

#include "types.hpp"
#include <string>
#include <vector>

class IDevice
{
    public:
        IDevice() { _deviceName = "IDevice"; }
        IDevice(std::string sName) : _deviceName(sName) {}
        virtual ~IDevice() {}

        // pure virtuals
		virtual Word OnAttach(Word nextAddr) = 0;
		virtual void OnInit() = 0;
		virtual void OnQuit() = 0;
		virtual void OnActivate() = 0;
		virtual void OnDeactivate() = 0;
		virtual void OnEvent(SDL_Event* evnt) = 0;
		virtual void OnUpdate(float fElapsedTime) = 0;
		virtual void OnRender() = 0;

        // virtuals
        virtual Byte read(Word offset, bool debug = false);
        virtual void write(Word offset, Byte data, bool debug = false);
        virtual Word read_word(Word offset, bool debug = false);
        virtual void write_word(Word offset, Word data, bool debug = false);  

        // helpers
        void DisplayEnum(std::string sToken, Word ofs, std::string sComment);

        // public accessors
        std::string name() { return _deviceName; }
        Word Base() { return m_base; }
        void Base(Word addr) { m_base = addr; }
        Word Size() { return m_size = m_memory.size(); }
        void Size(Word size) { m_size = size; m_memory.resize(size); }
        std::string Name()  { return _deviceName; }
        void Name(std::string n) { _deviceName = n; }
        Byte _memory(Word ofs) { return m_memory[ofs]; }
        void _memory(Word ofs, Byte data) { m_memory[ofs] = data; }

    protected:
        std::string _deviceName = "??DEV??";
        Uint16 m_base = 0;
        Uint16 m_size = 0;
        std::vector<Uint8> m_memory;	// memory mapped to this device  
};


/////////////////////
//
// Standard Memory Devices
//
////////

class RAM : public IDevice
{
    public:
        RAM() {  Name("RAM"); }
        RAM(std::string sName) { Name(sName); }
        virtual ~RAM() {}    

		Word OnAttach(Word nextAddr) override { return 0xCCCC; }
		void OnInit() override {}
		void OnQuit() override {}
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override {}
		void OnUpdate(float fElapsedTime) override {}
		void OnRender() override {} 
};

class ROM : public IDevice
{
    public:
        ROM() {  Name("ROM"); }
        ROM(std::string sName) { Name(sName); }
        virtual ~ROM() {}    

        virtual void write(Word offset, Byte data, bool debug = false);

		Word OnAttach(Word nextAddr) override { return 0xCCCC; }
		void OnInit() override {}
		void OnQuit() override {}
		void OnActivate() override {}
		void OnDeactivate() override {}
		void OnEvent(SDL_Event* evnt) override {}
		void OnUpdate(float fElapsedTime) override {}
		void OnRender() override {}         
};
