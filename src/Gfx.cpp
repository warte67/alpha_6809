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

    return data;
}

void Gfx::write(Word offset, Byte data, bool debug)
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    IDevice::write(offset,data);
    // printf("Gfx::read($%04X) = $%02X\n", offset,  IDevice::read(offset));
}

Word Gfx::OnAttach(Word nextAddr)
{
    // printf("%s::OnAttach()\n", Name().c_str());

    Word old_addr = nextAddr;
    DisplayEnum("GFX_REG", nextAddr, "(Byte) Testing First Gfx Register");
    nextAddr += 1;
    return nextAddr - old_addr;
}

void Gfx::OnInit()
{
    // printf("%s::OnInit()\n", Name().c_str());
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


