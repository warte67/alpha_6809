// *************************************************
// *
// * GfxCore.cpp
// *
// ***********************************

#include <sstream>
#include "GfxCore.hpp"
#include "Bus.hpp"


Byte GfxCore::read(Word offset, bool debug)
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    return data;
}

void GfxCore::write(Word offset, Byte data, bool debug)
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    IDevice::write(offset,data);
    // printf("Gfx::read($%04X) = $%02X\n", offset,  IDevice::read(offset));
}

Word GfxCore::OnAttach(Word nextAddr)
{
    // printf("%s::OnAttach()\n", Name().c_str());

    Word old_addr = nextAddr;
    DisplayEnum("VIDEO_START", nextAddr, "Start of Video Buffer Memory");
    nextAddr += VID_BUFFER_SIZE;
    DisplayEnum("VIDEO_END", nextAddr-1, "End of Video Buffer Memory");
    return nextAddr - old_addr;
}

void GfxCore::OnInit()
{
    // printf("%s::OnInit()\n", Name().c_str());
}

void GfxCore::OnQuit()
{
    // printf("%s::OnQuit()\n", Name().c_str());
}

void GfxCore::OnActivate()
{
    // printf("%s::OnActivate()\n", Name().c_str());

    if (!sdl_window)
    {
        sdl_window = SDL_CreateWindow("alpha_6809",
                              SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, 
                              512, 288,
                              SDL_WINDOW_RESIZABLE);
        if (!sdl_window)
        {
            std::stringstream ss;
            ss << "Unable to create the SDL window: " << SDL_GetError();
            Bus::Error(ss.str());
        }            
    }
}

void GfxCore::OnDeactivate()
{
    // printf("%s::OnDeactivate()\n", Name().c_str());

    // destroy the SDL Window
    if (sdl_window)
    {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }
}

void GfxCore::OnEvent(SDL_Event* evnt)
{
    // printf("%sw::OnEvent()\n", Name().c_str());
}

void GfxCore::OnUpdate(float fElapsedTime)
{
    // printf("%s::OnUpdate()\n", Name().c_str());
}

void GfxCore::OnRender()
{
    // printf("%s::OnRender()\n", Name().c_str());
}


