// *************************************************
// *
// * Debug.cpp
// *
// ***********************************

#include <sstream>
#include "Bus.hpp"
#include "Gfx.hpp"
#include "Debug.hpp"
#include "C6809.hpp"
#include "font8x8_system.hpp"

Byte Debug::read(Word offset, bool debug) 
{
    Byte data = IDevice::read(offset);
    printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    switch (offset)
    {
		case SYS_STATE: {
			Byte err = C6809::s_sys_state & 0xF0;
			C6809::s_sys_state &= 0x0F;
			data = C6809::s_sys_state | err; 
			break;
		}        
    }

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}
void Debug::write(Word offset, Byte data, bool debug) 
{
    printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    switch (offset)
    {
		case SYS_STATE: { 
			C6809::s_sys_state = data;
			break;
		}        
    }

    IDevice::write(offset,data);   // update any internal changes too
}

Word Debug::OnAttach(Word nextAddr) 
{
    // printf("%s::OnAttach()\n", Name().c_str());    
    Word old_addr = nextAddr;

    DisplayEnum("", 0, "System Hardware Registers:");

    DisplayEnum("", 0, "");
    DisplayEnum("SYS_STATE", nextAddr, " (Byte) System State Register");
	DisplayEnum("", 0, "SYS_STATE: ABCD.SSSS");
	DisplayEnum("", 0, "     A:0   = Error: Standard Buffer Overflow ");
	DisplayEnum("", 0, "     B:0   = Error: Extended Buffer Overflow ");
	DisplayEnum("", 0, "     C:0   = Error: Reserved ");
	DisplayEnum("", 0, "     D:0   = Error: Reserved ");
	DisplayEnum("", 0, "     S:$0  = CPU Clock  25 khz.");
	DisplayEnum("", 0, "     S:$1  = CPU Clock  50 khz.");
	DisplayEnum("", 0, "     S:$2  = CPU Clock 100 khz.");
	DisplayEnum("", 0, "     S:$3  = CPU Clock 200 khz.");
	DisplayEnum("", 0, "     S:$4  = CPU Clock 333 khz.");
	DisplayEnum("", 0, "     S:$5  = CPU Clock 416 khz.");
	DisplayEnum("", 0, "     S:$6  = CPU Clock 500 khz.");
	DisplayEnum("", 0, "     S:$7  = CPU Clock 625 khz.");
	DisplayEnum("", 0, "     S:$8  = CPU Clock 769 khz.");
	DisplayEnum("", 0, "     S:$9  = CPU Clock 833 khz.");
	DisplayEnum("", 0, "     S:$A  = CPU Clock 1.0 mhz.");
	DisplayEnum("", 0, "     S:$B  = CPU Clock 1.4 mhz.");
	DisplayEnum("", 0, "     S:$C  = CPU Clock 2.0 mhz.");
	DisplayEnum("", 0, "     S:$D  = CPU Clock 3.3 mhz.");
	DisplayEnum("", 0, "     S:$E  = CPU Clock 5.0 mhz.");
	DisplayEnum("", 0, "     S:$F  = CPU Clock ~10.0 mhz. (unmetered)");
	nextAddr++;

	DisplayEnum("", 0, "");
	DisplayEnum("SYS_SPEED", nextAddr, " (Word) Approx. Average CPU Clock Speed");
	nextAddr+=2;

	// DisplayEnum("", 0, "");
	DisplayEnum("SYS_CLOCK_DIV", nextAddr, " (Byte) 60 hz Clock Divider Register (Read Only) ");
	DisplayEnum("", 0, "SYS_CLOCK_DIV:");
	DisplayEnum("", 0, "     bit 7: 0.46875 hz");
	DisplayEnum("", 0, "     bit 6: 0.9375 hz");
	DisplayEnum("", 0, "     bit 5: 1.875 hz");
	DisplayEnum("", 0, "     bit 4: 3.75 hz");
	DisplayEnum("", 0, "     bit 3: 7.5 hz");
	DisplayEnum("", 0, "     bit 2: 15.0 hz");
	DisplayEnum("", 0, "     bit 1: 30.0 hz");
	DisplayEnum("", 0, "     bit 0: 60.0 hz");
	nextAddr++;

	DisplayEnum("", 0, "");
	DisplayEnum("SYS_TIMER", nextAddr, " (Word) Increments at 0.46875 hz");
	nextAddr += 2;



    
    DisplayEnum("", 0, "Debug Hardware Registers:");
    DisplayEnum("DBG_BEGIN",    nextAddr, "Start of Debug Hardware Registers");
    DisplayEnum("DBG_BRK_ADDR", nextAddr, "   (Word) Address of current breakpoint");
    nextAddr += 2;
    DisplayEnum("DBG_FLAGS",    nextAddr, "   (Byte) Debug Specific Hardware Flags:");
    nextAddr += 1;
    DisplayEnum("", 0, "    bit 7: Debug Enable");
    DisplayEnum("", 0, "    bit 6: Single Step Enable");
    DisplayEnum("", 0, "    bit 5: Clear All Breakpoints");
    DisplayEnum("", 0, "    bit 4: Update Breakpoint at DEBUG_BRK_ADDR");
    DisplayEnum("", 0, "    bit 3: FIRQ  (on low to high edge)");
    DisplayEnum("", 0, "    bit 2: IRQ   (on low to high edge)");
    DisplayEnum("", 0, "    bit 1: NMI   (on low to high edge)");
    DisplayEnum("", 0, "    bit 0: RESET (on low to high edge)");
    DisplayEnum("DBG_END", nextAddr, "End Debug Registers");
    DisplayEnum("", 0, "");

    return nextAddr - old_addr;
}

void Debug::OnInit() 
{
    // printf("%s::OnInit()\n", Name().c_str());    

    constexpr int DMONITOR = 2;

    s_bIsDebugActive = true;

    sdl_debug_window = SDL_CreateWindow("alpha_6809 Debugger",
                            SDL_WINDOWPOS_CENTERED_DISPLAY(DMONITOR),  SDL_WINDOWPOS_CENTERED_DISPLAY(DMONITOR), 
                            DEBUG_BUFFER_WIDTH, DEBUG_BUFFER_HEIGHT,
                            debug_window_flags);
    if (!sdl_debug_window)
    {
        std::stringstream ss;
        ss << "Unable to create the SDL debugger window: " << SDL_GetError();
        Bus::Error(ss.str());
    }            
	// create the renderer
    if (!sdl_debug_renderer)
    {
        sdl_debug_renderer = SDL_CreateRenderer(sdl_debug_window, -1, sdl_debug_renderer_flags);
        if (!sdl_debug_renderer)
            Bus::Error("Error Creating _renderer");	
        SDL_SetRenderDrawBlendMode(sdl_debug_renderer, SDL_BLENDMODE_NONE);   
        if (!sdl_debug_renderer)
            Bus::Error("Error Creating Debug Renderer");  
    }    

	// create the render target texture
    if (!sdl_debug_target_texture)
    {
        sdl_debug_target_texture = SDL_CreateTexture(sdl_debug_renderer, SDL_PIXELFORMAT_ARGB4444,
                SDL_TEXTUREACCESS_STREAMING, DEBUG_WIDTH, DEBUG_HEIGHT);
        if (!sdl_debug_target_texture)
            Bus::Error("Error Creating _render_target");    
    }     

    // create the character buffer
    // _db_bfr.reserve(DEBUG_BUFFER_SIZE);
    for (unsigned int t=0; t<DEBUG_BUFFER_SIZE; t++)
    {
        D_GLYPH dg;
        dg.chr = 'A';
        dg.attr = 0xf0;
        _db_bfr.push_back(dg);
    }
    Byte ci=0;
    Byte at=0xf0;
    for (auto& c : _db_bfr)
    {
        c.chr = ci++;
        c.attr = at;
    }    
    //printf("_db_bfr.size(): %d\n", (int)_db_bfr.size());
}
void Debug::OnQuit() 
{
    // printf("%s::OnQuit()\n", Name().c_str());    
        // destroy the render target texture

    if (sdl_debug_target_texture)
    {
        SDL_DestroyTexture(sdl_debug_target_texture);
        sdl_debug_target_texture = nullptr;
    }
    // destroy the renderer
    if (sdl_debug_renderer)
    {
        SDL_DestroyRenderer(sdl_debug_renderer);
        sdl_debug_renderer = nullptr;
    }
    // destroy the SDL Window
    if (sdl_debug_window)
    {
        SDL_DestroyWindow(sdl_debug_window);
        sdl_debug_window = nullptr;
    }
}

void Debug::OnEvent(SDL_Event* evnt) 
{
    // printf("%s::OnEvent()\n", Name().c_str());   
    if (evnt->window.windowID != GetWindowID())
        return;

    switch (evnt->type) {
        // handle default events SDL_QUIT and ALT-X quits

        case SDL_WINDOWEVENT:
        {
            switch (evnt->window.event)
            {
                case SDL_WINDOWEVENT_CLOSE:
                {
                    printf("%s::OnEvent(): SDL_WINDOWEVENT_CLOSE\n", Name().c_str());
                    s_bIsDebugActive = false;
                    // SDL_HideWindow(sdl_debug_window);
                    SDL_MinimizeWindow(sdl_debug_window);   // just minimize instead of close
                    break;
                }                
                case SDL_WINDOWEVENT_MINIMIZED:
                {
                    printf("%s::OnEvent(): SDL_WINDOWEVENT_MINIMIZED\n", Name().c_str());
                    s_bIsDebugActive = false;
                    break;
                }
                case SDL_WINDOWEVENT_RESTORED:
                {
                    printf("%s::OnEvent(): SDL_WINDOWEVENT_RESTORED\n", Name().c_str());
                    s_bIsDebugActive = true;
                    break;
                }
            }
            break;
        }
        case SDL_QUIT:
            // handling of close button
            printf("DEBUG:  SDL_QUIT\n");
            // s_bIsRunning = false;
            break;
    }
}

void Debug::OnUpdate(float fElapsedTime) 
{
    // printf("%s::OnUpdate()\n", Name().c_str());   

    if (!IsActive())    return;

    SDL_SetRenderTarget(sdl_debug_renderer, sdl_debug_target_texture);
    SDL_SetRenderDrawColor(sdl_debug_renderer, 0,0,0,255);
	SDL_RenderClear(sdl_debug_renderer);	

    for (auto& c : _db_bfr)
    {
        c.chr++;
        if (c.chr % 32 == 0)
            c.attr++;
    }          

    _updateDebugScreen();

    // void *pixels;
    // int pitch;
    // if (SDL_LockTexture(sdl_debug_target_texture, NULL, &pixels, &pitch) < 0) {
    //     SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError());
    //     Bus::Error("");
    // }
    // else
    // {
    //     for (int y=0; y<=DEBUG_HEIGHT; y++)
    //     {
    //         for (int x=0; x<=DEBUG_BUFFER_WIDTH; x++)
    //         {
    //             Byte c = rand() % 256;
    //             _setPixel_unlocked(pixels, pitch, x, y, c, true);
    //         }
    //     }
    //     SDL_UnlockTexture(sdl_debug_target_texture);
    // }    
}

void Debug::OnRender() 
{
    if (!IsActive())    return;

    SDL_SetRenderTarget(sdl_debug_renderer, NULL);
    // SDL_SetRenderDrawColor(sdl_debug_renderer, 0,0,0,0);
	// SDL_RenderClear(sdl_debug_renderer);	
    SDL_RenderCopy(sdl_debug_renderer, sdl_debug_target_texture, NULL, NULL);


    SDL_RenderPresent(sdl_debug_renderer);
}


void Debug::_setPixel(int x, int y, Byte color_index, 
						SDL_Texture* _texture, bool bIgnoreAlpha)
{
    void *pixels;
    int pitch;
    if (SDL_LockTexture(_texture, NULL, &pixels, &pitch) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError());
        Bus::Error("");
    }
    else
    {
        _setPixel_unlocked(pixels, pitch, x, y, color_index, bIgnoreAlpha);
        SDL_UnlockTexture(_texture);
    }    
}

void Debug::_setPixel_unlocked(void* pixels, int pitch, int x, int y, Byte color_index, bool bIgnoreAlpha)
{
    Gfx* gfx = Bus::GetGfx();
    Uint16 *dst = (Uint16*)((Uint8*)pixels + (y * pitch) + (x*sizeof(Uint16)));		// because data size is two bytes 
    bool ALPHA_BLEND = true;
    if (ALPHA_BLEND)
    {       
        // int ret = ((p1 * (256-a))) + (p2 * (a+1)) >> 8;
        Uint16 pixel = *dst;	// 0xARGB
		Byte r1 = (pixel & 0x0f00) >> 8;
		Byte g1 = (pixel & 0x00f0) >> 4;
		Byte b1 = (pixel & 0x000f) >> 0;
		//
        Byte a2 = gfx->alf(color_index);
        Byte r2 = gfx->red(color_index);
        Byte g2 = gfx->grn(color_index);
        Byte b2 = gfx->blu(color_index);
        if (bIgnoreAlpha)
            a2 = 15;
		//
        Byte r = (((r1 * (16-a2))) + (r2 * (a2+1))) >> 4;
        Byte g = (((g1 * (16-a2))) + (g2 * (a2+1))) >> 4;
        Byte b = (((b1 * (16-a2))) + (b2 * (a2+1))) >> 4;

        if (gfx->alf(color_index) != 0 || bIgnoreAlpha)
        {
            *dst = (
                0xF000 | 
                (r<<8) | 
                (g<<4) | 
                (b)
            );          
		}	
    }
    else
    {        
        // simple non-zero alpha channel
        if (gfx->alf(color_index) != 0 || bIgnoreAlpha)
        {
            *dst = 
            (
                0xF000 |
                (gfx->red(color_index)<<8) |
                (gfx->grn(color_index)<<4) |
                gfx->blu(color_index)
            );    
        }
    }    
}


void Debug::_updateDebugScreen() 
{
    void *pixels;
    int pitch;

    if (SDL_LockTexture(sdl_debug_target_texture, NULL, &pixels, &pitch) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError());
        Bus::Error("");
    }
    else
    {
        Byte width = DEBUG_WIDTH / 8;
        // int i=0;
        Word i_size = _db_bfr.size();
        for (Word i=0; i<i_size; i++)
        {
            Byte ch = _db_bfr[i].chr;
            Byte at = _db_bfr[i].attr;
			Byte fg = at >> 4;
			Byte bg = at & 0x0f;
            int x = (i % width) * 8;
            int y = (i / width) * 8;
			for (int v = 0; v < 8; v++)
			{
				for (int h = 0; h < 8; h++)
				{
					int color = bg;
                    Byte gd = Gfx::GetGlyphData(ch, v);                    
					if (gd & (1 << (7 - h)))
						color = fg;
					_setPixel_unlocked(pixels, pitch, x + h, y + v, color);
				}
			}
        }
        SDL_UnlockTexture(sdl_debug_target_texture); 
    }
} 





bool Debug::SingleStep()
{
    // do nothing if singlestep is disabled
    if (!s_bSingleStep)
        return true;
    // wait for space
    if (s_bIsStepPaused)
        return false;
    return true;
}

void Debug::ContinueSingleStep() {
    // C6809* cpu = Bus::Inst().m_cpu;
    C6809* cpu = Bus::GetC6809();
    // if breakpoint reached... enable singlestep
    if (mapBreakpoints[cpu->getPC()] == true)
    {
        s_bIsDebugActive = true;
        s_bSingleStep = true;
    }
    // continue from paused state?
    s_bIsStepPaused = s_bSingleStep;
}






