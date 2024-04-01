// *************************************************
// *
// * Gfx.cpp
// *
// ***********************************

#include <sstream>
#include "Gfx.hpp"
#include "Bus.hpp"


Byte Gfx::read(Word offset, bool debug)
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    if (offset == GFX_MODE)
        return s_gfx_mode;

    return data;
}

void Gfx::write(Word offset, Byte data, bool debug)
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    if (offset == GFX_MODE)
    {
        s_gfx_mode = data;
    }

    IDevice::write(offset,data);
    // printf("Gfx::read($%04X) = $%02X\n", offset,  IDevice::read(offset));
}

Word Gfx::OnAttach(Word nextAddr)
{
    // printf("%s::OnAttach()\n", Name().c_str());

    Word old_addr = nextAddr;
    DisplayEnum("GFX_MODE", nextAddr, "(Byte) Graphics Mode");
	DisplayEnum("", 0, "\t     - bit 0-4   = Resolution Modes 0-31");
	DisplayEnum("", 0, "\t     - bit 5     = 0:windowed, 1:fullscreen (emulation only)");
	DisplayEnum("", 0, "\t     - bit 6     = 0:vsync off, 1:vsync on");
	DisplayEnum("", 0, "\t     - bit 7     = 0:text,  1:bitmap");

    nextAddr += 1;
    return nextAddr - old_addr;
}

void Gfx::OnInit()
{
    // printf("%s::OnInit()\n", Name().c_str());
    // Bus::Write(GFX_MODE, 0x12);
    // printf("GFX_MODE: $%02X\n", s_gfx_mode);
}

void Gfx::OnQuit()
{
    // printf("%s::OnQuit()\n", Name().c_str());
}

void Gfx::OnActivate()
{
    // printf("%s::OnActivate()\n", Name().c_str());
}

void Gfx::OnDeactivate()
{
    // printf("%s::OnDeactivate()\n", Name().c_str());
}

void Gfx::OnEvent(SDL_Event* evnt)
{
    // printf("%sw::OnEvent()\n", Name().c_str());
}

void Gfx::OnUpdate(float fElapsedTime)
{
    // printf("%s::OnUpdate()\n", Name().c_str());
}

void Gfx::OnRender()
{
    // printf("%s::OnRender()\n", Name().c_str());
}


