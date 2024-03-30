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
    // printf("Gfx::read($%04X) = $%02X\n", offset,  data);

    return data;
}

void Gfx::write(Word offset, Byte data, bool debug)
{
    // printf("Gfx::write($%04X, $%02X)\n", offset, data);    

    IDevice::write(offset,data);
    // printf("Gfx::read($%04X) = $%02X\n", offset,  IDevice::read(offset));
}

Word Gfx::OnAttach(Word nextAddr)
{
    // printf("%s::OnAttach()\n", Name().c_str());

    Word old_addr = nextAddr;
    DisplayEnum("VIDEO_START", nextAddr, "Start of Video Buffer Memory");
    nextAddr += VID_BUFFER_SIZE;
    DisplayEnum("VIDEO_END", nextAddr-1, "End of Video Buffer Memory");
    return nextAddr - old_addr;
}

void Gfx::OnInit()
{
    printf("%s::OnInit()\n", Name().c_str());

    // int d = 0;
    // for (int a=0; a<0x410; a++)
    //     Bus::Write(a, d++);
}

void Gfx::OnQuit()
{
    printf("%s::OnQuit()\n", Name().c_str());
}

void Gfx::OnActivate()
{
    // printf("Gfx::OnActivate()\n");

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

void Gfx::OnDeactivate()
{
    // printf("Gfx::OnDeactivate()\n");

    // destroy the SDL Window
    if (sdl_window)
    {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }
}

void Gfx::OnEvent(SDL_Event* evnt)
{
    // printf("Gfx::OnEvent()\n");
}

void Gfx::OnUpdate(float fElapsedTime)
{
    // printf("Gfx::OnUpdate()\n");
}

void Gfx::OnRender()
{
    // printf("Gfx::OnRender()\n");
}


