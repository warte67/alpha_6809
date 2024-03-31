// *************************************************
// *
// * GfxVRam.cpp
// *
// ***********************************

#include <sstream>
#include "GfxVRam.hpp"
#include "Bus.hpp"


Byte GfxVRam::read(Word offset, bool debug)
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    return data;
}

void GfxVRam::write(Word offset, Byte data, bool debug)
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    IDevice::write(offset,data);
    // printf("Gfx::read($%04X) = $%02X\n", offset,  IDevice::read(offset));
}

Word GfxVRam::OnAttach(Word nextAddr)
{
    // printf("%s::OnAttach()\n", Name().c_str());

    Word old_addr = nextAddr;
    DisplayEnum("VIDEO_START", nextAddr, "Start of Video Buffer Memory");
    nextAddr += VID_BUFFER_SIZE;
    DisplayEnum("VIDEO_END", nextAddr-1, "End of Video Buffer Memory");
    return nextAddr - old_addr;
}

void GfxVRam::OnInit()
{
    // printf("%s::OnInit()\n", Name().c_str());
}

void GfxVRam::OnQuit()
{
    // printf("%s::OnQuit()\n", Name().c_str());
}

void GfxVRam::OnActivate()
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

void GfxVRam::OnDeactivate()
{
    // printf("%s::OnDeactivate()\n", Name().c_str());

    // destroy the SDL Window
    if (sdl_window)
    {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }
}

void GfxVRam::OnEvent(SDL_Event* evnt)
{
    // printf("%sw::OnEvent()\n", Name().c_str());
}

void GfxVRam::OnUpdate(float fElapsedTime)
{
    // printf("%s::OnUpdate()\n", Name().c_str());
}

void GfxVRam::OnRender()
{
    // printf("%s::OnRender()\n", Name().c_str());
}


