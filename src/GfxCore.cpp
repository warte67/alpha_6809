// *************************************************
// *
// * GfxCore.cpp
// *
// ***********************************

#include <sstream>
#include "GfxCore.hpp"
#include "Gfx.hpp"
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

    _init_gmodes();
}

void GfxCore::OnQuit()
{
    // printf("%s::OnQuit()\n", Name().c_str());
}


void GfxCore::_decode_gmode()
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
        window_width = vec_timings[vec_gmodes[gmode & 0x1f].Timing_index].Width;
        window_height = vec_timings[vec_gmodes[gmode & 0x1f].Timing_index].Height;
    }

	// SDL Renderer Flags
	sdl_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	if (gmode & 0x40)
		sdl_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC;

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
        Bus::Write(GFX_MODE, gmode);
    }
    if (!(gmode & 0x80))
        bits_per_pixel = 0; // text only

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
    }
}

void GfxCore::OnActivate()
{
    // printf("%s::OnActivate()\n", Name().c_str());

    _decode_gmode();

    // dont create a new window if one already exists
    if (sdl_window)
    {
        SDL_SetWindowSize(sdl_window, window_width, window_height);
    }
    else
    {
        sdl_window = SDL_CreateWindow("alpha_6809",
                                SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, 
                                window_width, window_height,
                                window_flags);
        if (!sdl_window)
        {
            std::stringstream ss;
            ss << "Unable to create the SDL window: " << SDL_GetError();
            Bus::Error(ss.str());
        }            
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
}

void GfxCore::OnDeactivate()
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

void GfxCore::OnEvent(SDL_Event* evnt)
{
    // printf("%sw::OnEvent()\n", Name().c_str());

    switch(evnt->type)
    {
        case SDL_WINDOWEVENT:  
        {
            if (evnt->window.event == SDL_WINDOWEVENT_RESIZED)
            {
                window_width = evnt->window.data1;
                window_height = evnt->window.data2;
				// m_debug->_onWindowResize();
            }
			break;
		}
    }
}

void GfxCore::OnUpdate(float fElapsedTime)
{
    // printf("%s::OnUpdate()\n", Name().c_str());

    SDL_SetRenderTarget(sdl_renderer, sdl_target_texture);
	// SDL_SetRenderDrawColor(sdl_renderer, 8,32,4,255);    //rgba
    // SDL_RenderClear(sdl_renderer);    


    _updateTextScreen();
    return;

    void *pixels;
    int pitch;

    if (SDL_LockTexture(sdl_target_texture, NULL, &pixels, &pitch) < 0) {
        std::stringstream ss;
        ss << "Couldn't lock texture: " << SDL_GetError();
        Bus::Error(ss.str());
    }
    else
    {
        static Byte color = 0;
        for (int y=0; y<res_height;y++)
        {
            for (int x=0; x<res_width; x++)
            {
                _setPixel_unlocked(pixels, pitch, x, y, color++);
            }
        }
        // color++;

        // for (int t=0; t< 10000; t++)
        // {
        //     int x = rand() % res_width;
        //     int y = rand() % res_height;
        //     Byte color = rand() % 256;
        //     _setPixel_unlocked(pixels, pitch, x, y, color);
        // }
        SDL_UnlockTexture(sdl_target_texture); 
    }


}

void GfxCore::OnRender()
{
    // printf("%s::OnRender()\n", Name().c_str());

    SDL_SetRenderTarget(sdl_renderer, NULL);
    // SDL_RenderCopy(sdl_renderer, sdl_target_texture, NULL, NULL);

    // render aspect
    Byte mode = Bus::Read(GFX_MODE) & 0x1f;
    Byte tindex = vec_gmodes[mode].Timing_index;   

    float aspect = (float)vec_timings[tindex].Width  / (float)vec_timings[tindex].Height;
    // float aspect = (float)res_width  / (float)res_height;
    // float aspect = (float)window_width  / (float)window_height;
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


// helpers

void GfxCore::_init_gmodes()
{
    // Timings
    vec_timings.push_back( { 1280,  800 } );                //  0 = 1280 x 800
    vec_timings.push_back( { 1280,  720 } );                //  1 = 1280 x 720
    vec_timings.push_back( { 1024,  768 } );                //  2 = 1024 x 768
    vec_timings.push_back( {  640,  480 } );                //  3 =  640 x 480

    // graphics modes (*text mode only)
    vec_gmodes.push_back( { 0, 640, 400, 2, 2 } );         //   0*
    vec_gmodes.push_back( { 0, 640, 200, 2, 4 } );         //   1*
    vec_gmodes.push_back( { 0, 320, 400, 4, 2 } );         //   2*
    vec_gmodes.push_back( { 0, 320, 200, 4, 4 } );         //   3
    vec_gmodes.push_back( { 0, 320, 160, 4, 5 } );         //   4
    vec_gmodes.push_back( { 0, 256, 200, 5, 4 } );         //   5
    vec_gmodes.push_back( { 0, 256, 160, 5, 5 } );         //   6
    vec_gmodes.push_back( { 0, 160, 100, 8, 8 } );         //   7
    vec_gmodes.push_back( { 1, 640, 360, 2, 2 } );         //   8*
    vec_gmodes.push_back( { 1, 640, 240, 2, 3 } );         //   9*
    vec_gmodes.push_back( { 1, 320, 180, 4, 4 } );         //   10
    vec_gmodes.push_back( { 1, 320, 144, 4, 5 } );         //   11
    vec_gmodes.push_back( { 1, 256, 180, 5, 4 } );         //   12
    vec_gmodes.push_back( { 1, 256, 144, 5, 5 } );         //   13
    vec_gmodes.push_back( { 1, 256, 120, 5, 6 } );         //   14
    vec_gmodes.push_back( { 1, 160,  90, 8, 8 } );         //   15
    vec_gmodes.push_back( { 2, 512, 384, 2, 2 } );         //   16*
    vec_gmodes.push_back( { 2, 340, 256, 3, 3 } );         //   17
    vec_gmodes.push_back( { 2, 256, 256, 4, 3 } );         //   18
    vec_gmodes.push_back( { 2, 256, 192, 4, 4 } );         //   19
    vec_gmodes.push_back( { 2, 204, 152, 5, 5 } );         //   20
    vec_gmodes.push_back( { 2, 170, 128, 6, 6 } );         //   21
    vec_gmodes.push_back( { 2, 128, 128, 8, 6 } );         //   22
    vec_gmodes.push_back( { 2, 128,  96, 8, 8 } );         //   23
    vec_gmodes.push_back( { 3, 320, 240, 2, 2 } );         //   24
    vec_gmodes.push_back( { 3, 320, 160, 2, 3 } );         //   25
    vec_gmodes.push_back( { 3, 160, 160, 4, 3 } );         //   26
    vec_gmodes.push_back( { 3, 160, 120, 4, 4 } );         //   27
    vec_gmodes.push_back( { 3, 160,  96, 4, 5 } );         //   28
    vec_gmodes.push_back( { 3, 128, 120, 5, 4 } );         //   29
    vec_gmodes.push_back( { 3, 128,  96, 5, 5 } );         //   30
    vec_gmodes.push_back( { 3, 128,  80, 5, 6 } );         //   31

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







void GfxCore::_updateTextScreen() 
{
    if (true)
    {
        for (Word t=VIDEO_START; t<=VIDEO_END; t++)
            Bus::Write(t, rand()%256);
    }


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
					if (Gfx::GetGlyphData(ch, v) & (1 << (7 - h)))
						color = fg;
					// _setPixel_unlocked(pixels, pitch, x + h, y + v, 15);
					_setPixel_unlocked(pixels, pitch, x + h, y + v, color);
				}
			}
		}
        SDL_UnlockTexture(sdl_target_texture); 
    }
} 

void GfxCore::_setPixel(int x, int y, Byte color_index, 
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

void GfxCore::_setPixel_unlocked(void* pixels, int pitch, int x, int y, Byte color_index, bool bIgnoreAlpha)
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
