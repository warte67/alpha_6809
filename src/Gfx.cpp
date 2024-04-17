// *************************************************
// *
// * Gfx.cpp
// *
// ***********************************

#include <sstream>
#include "Gfx.hpp"
#include "Debug.hpp"
#include "Bus.hpp"
#include "C6809.hpp"
#include "font8x8_system.hpp"
#include "Memory.hpp"
#include "MemBank.hpp"

Byte Gfx::read(Word offset, bool debug)
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);
    switch (offset)
    {
        case GFX_MODE:          data = s_gfx_mode; Bus::Write(MEM_DSP_FLAGS, Bus::Read(MEM_DSP_FLAGS)); break;
        case GFX_EMU:           data = s_gfx_emu; break;

		case GFX_VID_END + 0: 	data = (gfx_vid_end >> 8) & 0xFF; break;
		case GFX_VID_END + 1: 	data = gfx_vid_end & 0xFF; break;

		case GFX_HRES + 0: 	
        {
            Word width = res_width;
            if (!(s_gfx_mode & 0x80)) // text mode
                width /= 8;
            data = (width >> 8) & 0xFF; 
            break;
        }
		case GFX_HRES + 1: 	
        {
            Word width = res_width;
            if (!(s_gfx_mode & 0x80)) // text mode
                width /= 8;
            data = width & 0xFF; 
            break;
        }
		case GFX_VRES + 0: 	
        {
            Word height = res_height;
            if (!(s_gfx_mode & 0x80)) // text mode
                height /= 8;
            data = (height >> 8) & 0xFF; 
            break;
        }
		case GFX_VRES + 1: 	
        {
            Word height = res_height;
            if (!(s_gfx_mode & 0x80)) // text mode
                height /= 8;
            data = height & 0xFF; 
            break;
        }

   		case GFX_PAL_IDX:   	data = _gfx_pal_idx; break;
		case GFX_PAL_CLR + 0: 	data = (_palette[_gfx_pal_idx].color >> 8) & 0xFF; break;
		case GFX_PAL_CLR + 1: 	data = _palette[_gfx_pal_idx].color & 0xFF; break;

        // text glyph definition data registers
        case GFX_GLYPH_IDX:     data = _gfx_glyph_idx; break;
        case GFX_GLYPH_DATA+0:  data = _gfx_glyph_data[_gfx_glyph_idx][0]; break;
        case GFX_GLYPH_DATA+1:  data = _gfx_glyph_data[_gfx_glyph_idx][1]; break;
        case GFX_GLYPH_DATA+2:  data = _gfx_glyph_data[_gfx_glyph_idx][2]; break;
        case GFX_GLYPH_DATA+3:  data = _gfx_glyph_data[_gfx_glyph_idx][3]; break;
        case GFX_GLYPH_DATA+4:  data = _gfx_glyph_data[_gfx_glyph_idx][4]; break;
        case GFX_GLYPH_DATA+5:  data = _gfx_glyph_data[_gfx_glyph_idx][5]; break;
        case GFX_GLYPH_DATA+6:  data = _gfx_glyph_data[_gfx_glyph_idx][6]; break;
		case GFX_GLYPH_DATA+7:  data = _gfx_glyph_data[_gfx_glyph_idx][7]; break;

        default:
            break;
    }

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}

void Gfx::write(Word offset, Byte data, bool debug)
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    switch (offset)
    {
        case GFX_MODE:
            if (data != s_gfx_mode)
            {
                // ToDo: Only change data if valid GMODE
                if (VerifyGmode(data))
                    s_gfx_mode = data;
                Bus::IsDirty(true);
            }
            break;
        case GFX_EMU:
            if (data != s_gfx_emu)
            {
                s_gfx_emu = data;
                Bus::IsDirty(true);
            }
            break;

		case GFX_PAL_IDX:	{ _gfx_pal_idx = data;  break; }
		case GFX_PAL_CLR+1:	{
			Word c = _palette[_gfx_pal_idx].color & 0xff00;
			_palette[_gfx_pal_idx].color = c | data;
			data |= _palette[_gfx_pal_idx].color;
			// Bus::IsDirty(true);
			break;
		}
		case GFX_PAL_CLR+0:	{
			Word c = _palette[_gfx_pal_idx].color & 0x00ff;
			_palette[_gfx_pal_idx].color = c | ((Word)data << 8);
			data = _palette[_gfx_pal_idx].color;
			// Bus::IsDirty(true);
			break;
		}
        // text glyph definition data registers
        case GFX_GLYPH_IDX:     _gfx_glyph_idx = data; break;
        case GFX_GLYPH_DATA+0:  _gfx_glyph_data[_gfx_glyph_idx][0] = data; break;
        case GFX_GLYPH_DATA+1:  _gfx_glyph_data[_gfx_glyph_idx][1] = data; break;
        case GFX_GLYPH_DATA+2:  _gfx_glyph_data[_gfx_glyph_idx][2] = data; break;
        case GFX_GLYPH_DATA+3:  _gfx_glyph_data[_gfx_glyph_idx][3] = data; break;
        case GFX_GLYPH_DATA+4:  _gfx_glyph_data[_gfx_glyph_idx][4] = data; break;
        case GFX_GLYPH_DATA+5:  _gfx_glyph_data[_gfx_glyph_idx][5] = data; break;
        case GFX_GLYPH_DATA+6:  _gfx_glyph_data[_gfx_glyph_idx][6] = data; break;
        case GFX_GLYPH_DATA+7:  _gfx_glyph_data[_gfx_glyph_idx][7] = data; break; 		

    }

    IDevice::write(offset,data);
    // printf("Gfx::read($%04X) = $%02X\n", offset,  IDevice::read(offset));
}

Word Gfx::OnAttach(Word nextAddr)
{
    // printf("%s::OnAttach()\n", Name().c_str());

    Word old_addr = nextAddr;
    DisplayEnum("GFX_BEGIN", nextAddr, " Start of Graphics Hardware Registers");
    DisplayEnum("GFX_MODE", nextAddr, "(Byte) Graphics Mode");
	DisplayEnum("", 0, "\t     - bit 0-4   = Resolution Modes 0-31");
	DisplayEnum("", 0, "\t     - bit 5-6   = Bits-Per-Pixel (in bitmap mode)");
	DisplayEnum("", 0, "\t     - bit 7     = 0:text,  1:bitmap");
    DisplayEnum("", 0, "");
    nextAddr++;

    DisplayEnum("GFX_EMU", nextAddr, "(Byte) Emulation Flags");
	DisplayEnum("", 0, "\t     - bits 0-2  = Active Monitor 0-7");
	DisplayEnum("", 0, "\t     - bits 3-5  = Debug Monitor 0-7");
	DisplayEnum("", 0, "\t     - bit  6    = 0:vsync off, 1:vsync on");
	DisplayEnum("", 0, "\t     - bit  7    = 0:windowed, 1:fullscreen");
    DisplayEnum("", 0, "");
    nextAddr++;

    DisplayEnum("GFX_VID_END", nextAddr, " (Word Read Only) Top of Display Buffer");
    DisplayEnum("", 0, "Note: This will change to reflect the highest address of ");
    DisplayEnum("", 0, "    the currently running video display mode.");
    DisplayEnum("", 0, "");
    nextAddr += 2;

    DisplayEnum("GFX_HRES", nextAddr, " (Word Read Only) Horizontal Display Resolution");
    DisplayEnum("", 0, "Note: This will reflect the number of character columns for the ");
    DisplayEnum("", 0, "    text modes, but will reflect pixel columns for bitmap modes. ");
    DisplayEnum("", 0, "");
    nextAddr += 2;

    DisplayEnum("GFX_VRES", nextAddr, " (Word Read Only) Vertical Display Resolution");
    DisplayEnum("", 0, "Note: This will reflect the number of character rows for the ");
    DisplayEnum("", 0, "    text modes, but will reflect pixel rows for bitmap modes. ");
    DisplayEnum("", 0, "");
    nextAddr += 2;

    DisplayEnum("GFX_PAL_IDX", nextAddr, " (Byte) Color Palette Index");
    DisplayEnum("", 0, "GFX_PAL_IDX: 0-255");
    DisplayEnum("", 0, "Note: Use this register to set the index into theColor Palette. ");
    DisplayEnum("", 0, "      Set this value prior referencing color data (GFX_PAL_CLR).");
    DisplayEnum("", 0, "");
    nextAddr += 1;

    DisplayEnum("GFX_PAL_CLR", nextAddr, " (Word) Indexed Color Palette Data");
    DisplayEnum("", 0, "GFX_PAL_CLR: Color Data A4R4G4B4 format");
    DisplayEnum("", 0, "Note: This is the color data for an individual palette entry.");
    DisplayEnum("", 0, "    Write to DSP_PAL_IDX with the index within the color palette");
    DisplayEnum("", 0, "    prior to reading or writing the color data in the GFX_PAL_CLR register.");
    DisplayEnum("", 0, "");
    nextAddr += 2;

    DisplayEnum("GFX_GLYPH_IDX", nextAddr, " (Byte) Text Glyph Index");
    DisplayEnum("", 0, "GFX_GLYPH_IDX: 0-256");
    DisplayEnum("", 0, "Note: Set this register to index a specific text glyph. Set this value");
    DisplayEnum("", 0, "    prior to updating glyph pixel data.");
    DisplayEnum("", 0, "");
    nextAddr += 1;

    DisplayEnum("GFX_GLYPH_DATA", nextAddr, " (8-Bytes) Text Glyph Pixel Data Array");
    DisplayEnum("", 0, "GFX_GLYPH_DATA: 8 rows of binary encoded glyph pixel data");
    DisplayEnum("", 0, "Note: Each 8x8 text glyph is composed of 8 bytes. The first byte in this ");
    DisplayEnum("", 0, "    array represents the top line of 8 pixels. Each array entry represents");
    DisplayEnum("", 0, "    a row of 8 pixels. ");
    DisplayEnum("", 0, "");
    nextAddr += 8;

    DisplayEnum("GFX_END", nextAddr, " End of Graphics Hardware Registers");
    DisplayEnum("", 0, "");

    return nextAddr - old_addr;
}

void Gfx::_init_tests()
{
    // Bus::Write(GFX_PAL_IDX, 0x12);
    // printf("GFX_PAL_IDX: $%02X\n", _gfx_pal_idx);
}

void Gfx::OnInit()
{
    // printf("%s::OnInit()\n", Name().c_str());

    // _init_tests();


    _init_gmodes();

    // INITIALIZE PALETTE DATA
    if (_palette.size() == 0)
    {
        // BASIC COLORS (0-15) CUSTOM DEFAULT COLORS
        std::vector<PALETTE> ref = {    
			{ 0xF000 },		// 0: black
			{ 0xF555 },		// 1: dk gray
			{ 0xF007 },		// 2: dk blue
			{ 0xF600 },		// 3: dk red
			{ 0x0140 },		// 4: dk green
			{ 0xF840 },		// 5: brown
			{ 0xF406 },		// 6: purple          
			{ 0xF046 },		// 7: deep sea           	
			{ 0xF888 },		// 8: lt gray
			{ 0xF22F },		// 9: blue
			{ 0xFd00 },		// A: red
			{ 0xF4F6 },		// B: lt green
			{ 0xFED0 },		// C: yellow
			{ 0xF85b },		// D: Lt Purple
			{ 0xF59f },		// E: lt sky
			{ 0xFFFF },		// F: white

        // // BASIC COLORS (0-15) STANDARD CGA COLORS
        // std::vector<PALETTE> ref = {    
		// 	{ 0x0000 },	// 0: DK_BLACK
		// 	{ 0xF005 },	// 1: DK_BLUE
		// 	{ 0xF050 },	// 2: DK_GREEN
		// 	{ 0xF055 },	// 3: DK_CYAN
		// 	{ 0xF500 },	// 4: DK_RED
		// 	{ 0xF505 },	// 5: DK_MAGENTA
		// 	{ 0xF631 },	// 6: BROWN
		// 	{ 0xFAAA },	// 7: GRAY
		// 	{ 0xF555 },	// 8: DK_GRAY
		// 	{ 0xF00F },	// 9: BLUE
		// 	{ 0xF0F0 },	// a: GREEN
		// 	{ 0xF0FF },	// b: CYAN
		// 	{ 0xFF00 },	// c: RED
		// 	{ 0xFA0A },	// d: MAGENTA
		// 	{ 0xFFF0 },	// e: YELLOW
		// 	{ 0xFFFF },	// f: WHITE
        };
        for (auto &p : ref)
        {
			_palette.push_back(p);     
            // printf("%3d:$%04X\n", (int)_palette.size(), p.color);
        }

        // BGR 6x6x6 CUBE COLORS (16-232)          
        Byte b=0, g=0, r=0;
        for (float fb=0.0f; fb<=256.0f; fb+=(256.0f / 5.0f))
        {
            b = fb;
            if (fb == 256.0f) b=255;
            for (float fg=0.0f; fg<=256.0f; fg+=(256.0f / 5.0f))
            {
                g = fg;
                if (fg == 256.0f) g=255;
                for (float fr=0.0f; fr<=256.0f; fr+=(256.0f / 5.0f))
                {
                    r = fr;
                    if (fr == 256.0f) r=255;
                    PALETTE ent = { 0x0000 };
                    ent.a = 255>>4;
                    ent.b = b>>4;
                    ent.g = g>>4;
                    ent.r = r>>4;
                    _palette.push_back(ent);
                    // printf("%3d:$%04X\n", (int)_palette.size(), ent.color);
                }
            }
        }

        // GRAYSCALE RAMP (233-255) 
        for (float f=0.0f; f<=256.0f; f+=(256.0f / 24.0f))
        {
            if (f != 0.0f)
            {
                PALETTE ent = { 0x0000 };
                ent.a = 255>>4;
                ent.b = (int)f>>4;
                ent.g = (int)f>>4;
                ent.r = (int)f>>4;
                _palette.push_back(ent);
            }
        }  

        // Add one 100% transparent white entry to the end              
        _palette.push_back({0x0FFF});   // (255 = 100% transparent)

        // printf("_palette.size(): %3d\n", (int)_palette.size());
    }

    // initialize the font glyph buffer
    for (int i=0; i<256; i++)
        for (int r=0; r<8; r++)
            _gfx_glyph_data[i][r] = font8x8_system[i][r];

    // set the default monitor
    s_gfx_emu |= (MAIN_MONITOR & 0x07);

    // save the default palette
    SaveGimpPalette("retro_6809.gpl", "Retro 6809");
}

void Gfx::OnQuit()
{
    // printf("%s::OnQuit()\n", Name().c_str());
}

void Gfx::OnActivate()
{
    // printf("%s::OnActivate()\n", Name().c_str());
    _decode_gmode();

    // s_gfx_emu
    int MainMonitor = s_gfx_emu & 0x07;
    // printf("MainMonitor: %d\n", MainMonitor);

    // create the main window
    sdl_window = SDL_CreateWindow("alpha_6809",
        SDL_WINDOWPOS_CENTERED_DISPLAY(MainMonitor),  
        SDL_WINDOWPOS_CENTERED_DISPLAY(MainMonitor), 
        window_width, window_height,
        window_flags);
    if (!sdl_window)
    {
        std::stringstream ss;
        ss << "Unable to create the SDL window: " << SDL_GetError();
        Bus::Error(ss.str());
    }            
	// create the renderer
    if (!sdl_renderer)
    {
        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, sdl_renderer_flags);
        if (!sdl_renderer)
            Bus::Error("Error Creating _renderer");	
        SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);   
        if (!sdl_renderer)
            Bus::Error("Error Creating _renderer");  
    }    

	// create the render target texture
    if (!sdl_target_texture)
    {
        sdl_target_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ARGB4444,
                SDL_TEXTUREACCESS_STREAMING, res_width, res_height);
        if (!sdl_target_texture)
            Bus::Error("Error Creating _render_target");    
    }
    // C6809::IsCpuEnabled(true); 
}

void Gfx::OnDeactivate()
{
    // printf("%s::OnDeactivate()\n", Name().c_str());

    // destroy the render target texture
    if (sdl_target_texture)
    {
        SDL_DestroyTexture(sdl_target_texture);
        sdl_target_texture = nullptr;
    }
    // destroy the renderer
    if (sdl_renderer)
    {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = nullptr;
    }
    // destroy the SDL Window
    if (sdl_window)
    {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }
}

void Gfx::OnEvent(SDL_Event* evnt)
{
    // if not a main window event, just return now
    if (SDL_GetWindowFlags(Debug::GetSDLWindow()) & SDL_WINDOW_INPUT_FOCUS)
        return;    

    // printf("%sw::OnEvent()\n", Name().c_str());
    switch(evnt->type)
    {
        case SDL_WINDOWEVENT:  
        {
            if (evnt->window.windowID == GetWindowID())     // redundant?
            {
                if (evnt->window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    window_width = evnt->window.data1;
                    window_height = evnt->window.data2;
                    // m_debug->_onWindowResize();
                }
                // main window is closed
                if (evnt->window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    Bus::IsRunning(false);
                }
            }
            break;
        }

        case SDL_KEYDOWN:
        {
            // [ALT-X]
            if (evnt->key.keysym.sym == SDLK_x)
            {
                if (SDL_GetModState() & KMOD_ALT)
                    Bus::IsRunning(false);
            }
            // Testing [ALT-ENTER]
            if (evnt->key.keysym.sym == SDLK_RETURN && SDL_GetModState() & KMOD_ALT)
            {
                Byte data = Bus::Read(GFX_EMU);
                (data & 0x80) ? data &= 0x7f : data |= 0x80;
                Bus::Write(GFX_EMU, data);
                // s_bIsDirty = true;
            }

            // [ALT] [LEFT] & [RIGHT]
            SDL_Keymod mod = SDL_GetModState();
            if (mod & KMOD_ALT)
            {
                if (Bus::GetDebug()->IsCursorVisible() == false)
                {
                    if (evnt->key.keysym.sym == SDLK_RIGHT)
                        Bus::Write(GFX_MODE, Bus::Read(GFX_MODE)+1);
                    if (evnt->key.keysym.sym == SDLK_LEFT)
                        Bus::Write(GFX_MODE, Bus::Read(GFX_MODE)-1);
                    // Testing [UP] & [DOWN]
                    if (evnt->key.keysym.sym == SDLK_DOWN || evnt->key.keysym.sym == SDLK_UP)
                    {
                        Byte data = Bus::Read(GFX_MODE);
                        if (!(data & 0x80))
                        {
                            data &= 0x1F;
                            data |= 0x80;
                        }
                        else
                        {
                            if (evnt->key.keysym.sym == SDLK_UP)
                                data += 0x20;
                            else
                                data -= 0x20;
                        }
                        Bus::Write(GFX_MODE, data);
                        if (Bus::Read(GFX_MODE)!=data)
                        {
                            data &= 0x1f;
                            Bus::Write(GFX_MODE, data);
                        }
                    }
                }
            }
            
            break;
        }

    }
}

void Gfx::OnUpdate(float fElapsedTime)
{
    // printf("%s::OnUpdate()\n", Name().c_str());
    SDL_SetRenderTarget(sdl_renderer, sdl_target_texture);

    SDL_SetRenderDrawColor(sdl_renderer, 0,0,0,0);
	SDL_RenderClear(sdl_renderer);	

    if (Bus::Read(MEM_DSP_FLAGS) & 0x80) 
        _updateExtendedBitmapScreen();
    else
    {        
        SDL_Surface *surface = nullptr;
        SDL_LockTextureToSurface(sdl_target_texture, NULL, &surface);
        //               ARGB 
        Uint32 color = 0xF000;
        color |= (red(0)<<8);
        color |= (grn(0)<<4);
        color |= (blu(0)<<0);
        SDL_FillRect(surface, NULL, color);

        SDL_UnlockTexture(sdl_target_texture);
    }

    // is standard display enabled?
    if (Bus::Read(MEM_DSP_FLAGS) & 0x40)
    {
        if (bIsBitmapMode)
            _updateBitmapScreen();
        else
            _updateTextScreen();
    }

    return;
}

void Gfx::OnRender()
{
    // printf("%s::OnRender()\n", Name().c_str());
    SDL_SetRenderTarget(sdl_renderer, NULL);

    // render aspect
    Byte mode = Bus::Read(GFX_MODE) & 0x1f;
    Byte tindex = vec_gmodes[mode].Timing_index;   

    float aspect = (float)vec_timings[tindex].Width  / (float)vec_timings[tindex].Height;
    int ww = window_width;
    int wh = window_height;
    float fh = (float)window_height;
    float fw = (float)fh * aspect;
    if (fw > ww)
    {
        fw = ww;
        fh = fw / aspect;
    }
    SDL_Rect dest = 
    {
        int(window_width / 2 - (int)fw / 2), 
        int(wh / 2 - (int)fh / 2), 
        (int)fw, 
        (int)fh 
    };
    SDL_SetRenderDrawColor(sdl_renderer, 0,0,0,0);
	SDL_RenderClear(sdl_renderer);	
    SDL_RenderCopy(sdl_renderer, sdl_target_texture, NULL, &dest);
}


bool Gfx::VerifyGmode(Byte gmode)
{
    // Verify Graphics Mode
    int bp_size = (res_width * res_height) / 8;
    bits_per_pixel = 1<<((gmode & 0x60)>>5);
    bIsBitmapMode = false;
    if (gmode & 0x80)
    {
        bIsBitmapMode = true;
        // single bit plane size is too big to fit in video memory
        if (bp_size > VID_BUFFER_SIZE)
        {
            gmode &= 0x7f;  // clear the graphics mode bit
            bIsBitmapMode = false;
        }
        else
        {
            // correct bits_per_pixel
            if (bits_per_pixel > 1)
            {
                while (bp_size * bits_per_pixel > VID_BUFFER_SIZE)
                {
                    gmode -= 0x20;
                    bits_per_pixel = 1<<((gmode & 0x60)>>5);
                }
            }
        }
        IDevice::write(GFX_MODE, gmode);
    }
    if (!(gmode & 0x80))
        bits_per_pixel = 0; // text only   
    if (gmode == Bus::Read(GFX_MODE))    
        return false;
    return true;
}


Word Gfx::GetTimingWidth()   
{ return vec_timings[vec_gmodes[Bus::Read(GFX_MODE) & 0x1f].Timing_index].Width;  }
Word Gfx::GetTimingHeight()  
{ return vec_timings[vec_gmodes[Bus::Read(GFX_MODE) & 0x1f].Timing_index].Height; }

void Gfx::_decode_gmode()
{
    Byte gmode = Bus::Read(GFX_MODE);
    Byte gemu = Bus::Read(GFX_EMU);

    // fetch the desktop size
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);      // index 0 = primary display
    auto desktop_width = DM.w;
    auto desktop_height = DM.h;

    // texture size
    res_width = vec_gmodes[gmode & 0x1f].Res_Width;
    res_height = vec_gmodes[gmode & 0x1f].Res_Height;
    float aspect = (float)vec_timings[vec_gmodes[gmode & 0x1f].Timing_index].Width / 
                    (float)vec_timings[vec_gmodes[gmode & 0x1f].Timing_index].Height;

    // windowed or fullscreen
    bIsFullscreen = false;
    window_flags = SDL_WINDOW_RESIZABLE;
    window_width = desktop_width * 0.65f;
    window_height = (int)(float)window_width / aspect;
    if (gemu & 0x80)
    {   // FULLSCREEN
        bIsFullscreen = true;
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
        // window_width = vec_timings[vec_gmodes[gmode & 0x1f].Timing_index].Width;
        // window_height = vec_timings[vec_gmodes[gmode & 0x1f].Timing_index].Height;
        window_width = desktop_width;
        window_height = desktop_height;
    }

	// SDL Renderer Flags
	sdl_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	if (gmode & 0x40)
		sdl_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC;

    // Verify Graphics Mode
    VerifyGmode(gmode);

    // Video Buffer Size
    gfx_vid_end = VIDEO_START + ((res_width * res_height)/8)*bits_per_pixel;
    if (gfx_vid_end == VIDEO_START) // is a text mode
        gfx_vid_end = VIDEO_START + ((res_width/8)*(res_height/8))*2;
    gfx_vid_end--;  // prevent off by one errors        

    // VSYNC
    SDL_RenderSetVSync(sdl_renderer, (gemu & 0x40));

    // Main Monitor

    // Debug Monitor

    // Testing: report results
    if (true)
    {
        printf("\x1b[31m-=<###################################################>=-\n");
        printf("\x1b[37mMODE: $%02X\n", gmode);
        (bIsFullscreen) ? printf("Full Screen\n") : printf("Windowed\n");
        (bIsBitmapMode) ? printf("Bitmap Mode ") : printf("Text Mode ");
        if (bIsBitmapMode)
            printf("    %3d x%3d\n", res_width, res_height);
        else
            printf("    %3d x%3d\n", res_width/8, res_height/8);
        printf("Display Width:  %4d\n", desktop_width);
        printf("Display Height: %4d\n", desktop_height);
        printf("Window Width:  %4d\n", window_width);
        printf("Window Height: %4d\n", window_height);
        printf("Res Width:  %4d\n", res_width);
        printf("Res Height: %4d\n", res_height);
        printf("aspect: %f\n", aspect);
        printf("Bits Per Pixel: %d\n", bits_per_pixel);
        printf("GFX_VID_END: $%04X\n", gfx_vid_end);
        printf("HRES: %d    VRES: %d\n", Bus::Read_Word(GFX_HRES), Bus::Read_Word(GFX_VRES));        
    }
}

void Gfx::_init_gmodes()
{
    // Timings
    vec_timings.push_back( { 1280,  800 } );                //  0 = 1280 x 800
    vec_timings.push_back( { 1280,  720 } );                //  1 = 1280 x 720
    vec_timings.push_back( { 1024,  768 } );                //  2 = 1024 x 768
    vec_timings.push_back( {  640,  480 } );                //  3 =  640 x 480

    // graphics modes (*text mode only)
    vec_gmodes.push_back( { 0, 640, 400, 2, 2 } );         //   0x00    0*
    vec_gmodes.push_back( { 0, 640, 200, 2, 4 } );         //   0x01    1*
    vec_gmodes.push_back( { 0, 320, 400, 4, 2 } );         //   0x02    2*
    vec_gmodes.push_back( { 0, 320, 200, 4, 4 } );         //   0x03    3
    vec_gmodes.push_back( { 0, 320, 160, 4, 5 } );         //   0x04    4
    vec_gmodes.push_back( { 0, 256, 200, 5, 4 } );         //   0x05    5
    vec_gmodes.push_back( { 0, 256, 160, 5, 5 } );         //   0x06    6
    vec_gmodes.push_back( { 0, 160, 100, 8, 8 } );         //   0x07    7
    vec_gmodes.push_back( { 1, 640, 360, 2, 2 } );         //   0x08    8*
    vec_gmodes.push_back( { 1, 640, 240, 2, 3 } );         //   0x09    9*
    vec_gmodes.push_back( { 1, 320, 180, 4, 4 } );         //   0x0A    10
    vec_gmodes.push_back( { 1, 320, 144, 4, 5 } );         //   0x0B    11
    vec_gmodes.push_back( { 1, 256, 180, 5, 4 } );         //   0x0C    12
    vec_gmodes.push_back( { 1, 256, 144, 5, 5 } );         //   0x0D    13
    vec_gmodes.push_back( { 1, 256, 120, 5, 6 } );         //   0x0E    14
    vec_gmodes.push_back( { 1, 160,  90, 8, 8 } );         //   0x0F    15
    vec_gmodes.push_back( { 2, 512, 384, 2, 2 } );         //   0x10    16*
    vec_gmodes.push_back( { 2, 340, 256, 3, 3 } );         //   0x11    17
    vec_gmodes.push_back( { 2, 256, 256, 4, 3 } );         //   0x12    18
    vec_gmodes.push_back( { 2, 256, 192, 4, 4 } );         //   0x13    19
    vec_gmodes.push_back( { 2, 204, 152, 5, 5 } );         //   0x14    20
    vec_gmodes.push_back( { 2, 171, 128, 6, 6 } );         //   0x15    21
    vec_gmodes.push_back( { 2, 128, 128, 8, 6 } );         //   0x16    22
    vec_gmodes.push_back( { 2, 128,  96, 8, 8 } );         //   0x17    23
    vec_gmodes.push_back( { 3, 320, 240, 2, 2 } );         //   0x18    24
    vec_gmodes.push_back( { 3, 320, 160, 2, 3 } );         //   0x19    25
    vec_gmodes.push_back( { 3, 160, 160, 4, 3 } );         //   0x1A    26
    vec_gmodes.push_back( { 3, 160, 120, 4, 4 } );         //   0x1B    27
    vec_gmodes.push_back( { 3, 160,  96, 4, 5 } );         //   0x1C    28
    vec_gmodes.push_back( { 3, 128, 120, 5, 4 } );         //   0x1D    29
    vec_gmodes.push_back( { 3, 128,  96, 5, 5 } );         //   0x1E    30
    vec_gmodes.push_back( { 3, 128,  80, 5, 6 } );         //   0x1F    31

    if (false)
    {
        // verify gmodes
        int i=0;
        for (auto& a: vec_gmodes)
        {
            Byte timing_index = a.Timing_index;     // or (i/32);
            printf("I:%3d  RW:%3d  RH:%3d  PW:%2d RH:%2d  %4d x%4d", i, 
                a.Res_Width, a.Res_Height, a.Pixel_Width, a.Pixel_Height, 
                a.Res_Width*a.Pixel_Width, a.Res_Height*a.Pixel_Height);
            printf("    Timing: %2d  DW:%4d  DH:%4d", timing_index,
                vec_timings[timing_index].Width,
                vec_timings[timing_index].Height );
            Word pw = vec_timings[timing_index].Width - (a.Res_Width*a.Pixel_Width);
            Word ph = vec_timings[timing_index].Height - (a.Res_Height*a.Pixel_Height);
            if (pw != 0 || ph != 0)
                printf("  PADDING: %2d x%2d", pw, ph);
            printf("\n");            
            i++;
        }
    }
}

void Gfx::_updateBitmapScreen()
{
    bool ignore_alpha = true;
    if (Bus::Read(MEM_DSP_FLAGS) & 0x80) 
        ignore_alpha = false;

    Word pixel_index = VIDEO_START;
    void *pixels;
    int pitch;
    if (SDL_LockTexture(sdl_target_texture, NULL, &pixels, &pitch) < 0)
        Bus::Error("Failed to lock texture: ");	
    else
    {
        for (int y = 0; y < res_height; y++)
        {
            for (int x = 0; x < res_width; )
            {
                // 256 color mode
                if (bits_per_pixel == 8)
                {
                    Byte index = Bus::Read(pixel_index++);
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                }
                // 16 color mode
                else if (bits_per_pixel == 4)
                {
                    Byte data = Bus::Read(pixel_index++);
                    Byte index = (data >> 4);
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data & 0x0f);
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                }
                // 4 color mode
                else if (bits_per_pixel == 2)
                {
                    Byte data = Bus::Read(pixel_index++);
                    Byte index = (data >> 6) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 4) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 2) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 0) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                }
                // 2 color mode
                else if (bits_per_pixel == 1)
                {
                    Byte data = Bus::Read(pixel_index++);
                    Byte index = (data >> 7) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha); 
                    index = (data >> 6) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 5) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 4) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 3) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 2) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 1) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                    index = (data >> 0) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, ignore_alpha);   
                }
            }
        }
        SDL_UnlockTexture(sdl_target_texture); 
    }

    // SDL_SetRenderTarget(_renderer, _render_target);
    SDL_RenderCopy(sdl_renderer, sdl_target_texture, NULL, NULL);		
}



void Gfx::_updateTextScreen() 
{
    bool ignore_alpha = true;
    if (Bus::Read(MEM_DSP_FLAGS) & 0x80) 
        ignore_alpha = false;

    void *pixels;
    int pitch;

    if (SDL_LockTexture(sdl_target_texture, NULL, &pixels, &pitch) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError());
        Bus::Error("");
    }
    else
    {
        Byte col = res_height / 8;
        Byte row = res_width / 8;
        Word end = ((col*row)*2) + VIDEO_START;
        // Word end  = VIDEO_START+128;
		Word addr = VIDEO_START;
		for (; addr < end; addr += 2)
		{
			Byte ch = Bus::Read(addr, true);
			Byte at = Bus::Read(addr + 1, true);
			Byte fg = at >> 4;
			Byte bg = at & 0x0f;
			Word index = addr - VIDEO_START;
			Byte width = res_width / 8;
			int x = ((index / 2) % width) * 8;
			int y = ((index / 2) / width) * 8;
			for (int v = 0; v < 8; v++)
			{
				for (int h = 0; h < 8; h++)
				{
					int color = bg;
					// if (_gfx_glyph_data[ch][v] & (1 << 7 - h))
                    Byte gd = Gfx::GetGlyphData(ch, v);
					if (gd & (1 << (7 - h)))
						color = fg;
					// _setPixel_unlocked(pixels, pitch, x + h, y + v, 15);
					_setPixel_unlocked(pixels, pitch, x + h, y + v, color, ignore_alpha);
				}
			}
		}
        SDL_UnlockTexture(sdl_target_texture); 
    }
} 

void Gfx::_setPixel(int x, int y, Byte color_index, 
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

void Gfx::_setPixel_unlocked(void* pixels, int pitch, int x, int y, Byte color_index, bool bIgnoreAlpha)
{
    /*************************************************************
     * NOTES:
     * 
     *  When transitioning to use an extended 64k external graphics 
     *  buffer. This method should then include the alpha channel.
     *  As of this point, the bIgnoreAlpha flag is not functional.
     *  Use the Mouse::_setPixel_cursor() method as reference. That
     *  Method does work as expected.
     * 
     *************************************************************/

    Gfx* gfx = Bus::GetGfx();
    Uint16 *dst = (Uint16*)((Uint8*)pixels + (y * pitch) + (x*sizeof(Uint16)));		// because data size is two bytes 

    bool cls = false;
    if (color_index ==0 && bIgnoreAlpha)
        cls = true;

    if (!bIgnoreAlpha)
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
        // if (bIgnoreAlpha)
        //     a2 = 15;
		//
        Byte r = (((r1 * (16-a2))) + (r2 * (a2+1))) >> 4;
        Byte g = (((g1 * (16-a2))) + (g2 * (a2+1))) >> 4;
        Byte b = (((b1 * (16-a2))) + (b2 * (a2+1))) >> 4;

        if (gfx->alf(color_index) != 0) // || bIgnoreAlpha)
        {
            *dst = (
                0xF000 | 
                (r<<8) | 
                (g<<4) | 
                (b)
            );          
            // *dst = (
            //     (a2<<12) |       // utilize the alpha channel
            //     (r<<8) | 
            //     (g<<4) | 
            //     (b)
            // );          
		}	
    }
    else
    {        
        if (cls)
        {
            *dst = 0x0000;
        }
        else
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


void Gfx::_updateExtendedBitmapScreen()
{
    Memory* mm = Bus::GetMemory();

    Byte bpp = 1<<(Bus::Read(MEM_DSP_FLAGS) & 3);

    // // calc the display buffer size
    // Byte bpp = 1<<(Bus::Read(MEM_DSP_FLAGS)&3);
    // int size = ((res_width * res_height) / 8) * bpp;
    // mm->memory_btm = size;
    // while (size > 0xffff)
    // {
    //     Bus::Write(MEM_DSP_FLAGS, ((Bus::Read(MEM_DSP_FLAGS)-1)&3)|0x80);
    //     bpp = 1<<(Bus::Read(MEM_DSP_FLAGS)&3);
    //     size = ((res_width * res_height) / 8) * bpp;
    //     mm->memory_btm = size;
    // }
    // printf("BPP:%d  SIZE:$%04X\n", bpp, size);


    // display the extended bitmap buffer
    Word pixel_index = 0x0000;
    void *pixels;
    int pitch;
    if (SDL_LockTexture(sdl_target_texture, NULL, &pixels, &pitch) < 0)
        Bus::Error("Failed to lock texture: ");	
    else
    {
        for (int y = 0; y < res_height; y++)
        {
            for (int x = 0; x < res_width; )
            {
                // 256 color mode
                if (bpp == 8)
                {
// printf("256-colors\n");
                    Byte index = mm->ext_memory[pixel_index++];
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                }
                // 16 color mode
                else if (bpp == 4)
                {
// printf("16-colors\n");
                    Byte data = mm->ext_memory[pixel_index++];
                    Byte index = (data >> 4);
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data & 0x0f);
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                }
                // 4 color mode
                else if (bpp == 2)
                {
// printf("4-colors\n");
                    Byte data = mm->ext_memory[pixel_index++];
                    Byte index = (data >> 6) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 4) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 2) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 0) & 0x03;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                }
                // 2 color mode
                else if (bpp == 1)
                {
// printf("2-colors\n");
                    Byte data = mm->ext_memory[pixel_index++];
                    Byte index = (data >> 7) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true); 
                    index = (data >> 6) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 5) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 4) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 3) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 2) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 1) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                    index = (data >> 0) & 1;
                    _setPixel_unlocked(pixels, pitch, x++, y, index, true);   
                }
            }
        }
        SDL_UnlockTexture(sdl_target_texture); 
    }

    // SDL_SetRenderTarget(_renderer, _render_target);
    SDL_RenderCopy(sdl_renderer, sdl_target_texture, NULL, NULL);		
}


// save the current palette to a GIMP (*.gpl) palette
bool Gfx::SaveGimpPalette(const std::string& file, const std::string& name)
{
    FILE* fp = fopen(file.c_str(), "wb");
    if (fp)
    {
        // Save the Header
        fprintf(fp, "GIMP Palette\n");
        fprintf(fp, "Name: %s\n", name.c_str());
        fprintf(fp, "Columns: 0\n");
        fprintf(fp, "# Palette Data\n");

        // Write the palette data
        for (int idx=0; idx<256; idx++)
        {
            Byte red = (_palette[idx].r << 4) | _palette[idx].r;
            Byte grn = (_palette[idx].g << 4) | _palette[idx].g;
            Byte blu = (_palette[idx].b << 4) | _palette[idx].b;
            fprintf(fp, "%3d  %3d  %3d  Index_%03d\n", red, grn, blu, idx);
        }

        // close the file
        fclose(fp);
        return true;
    } 
    return false;
}

// load from a GIMP (*.gpl) formatted palette file
bool Gfx::LoadGimpPalette(const std::string& filename)
{
    return true;
}

