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
    printf("Gfx::OnAttach()\n");
    return 0;
}

void Gfx::OnInit()
{
    printf("Gfx::OnInit()\n");

    // int d = 0;
    // for (int a=0; a<0x410; a++)
    //     Bus::Write(a, d++);
}

void Gfx::OnQuit()
{
    printf("Gfx::OnQuit()\n");
}

void Gfx::OnActivate()
{
    // printf("Gfx::OnActivate()\n");

    if (!sdl_window)
    {
        sdl_window = SDL_CreateWindow("alpha_6809",
                              SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, 
                              1024, 576,
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


