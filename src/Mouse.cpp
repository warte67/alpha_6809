// *************************************************
// *
// * Mouse.cpp
// *
// ***********************************

#include "Bus.hpp"
#include "Gfx.hpp"
#include "Debug.hpp"
#include "Mouse.hpp"

Byte Mouse::read(Word offset, bool debug) 
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);
    switch (offset)
    {
        case CSR_XPOS + 0:      data = mouse_x >> 8; break;
        case CSR_XPOS + 1:      data = mouse_x & 0xFF; break;
        case CSR_YPOS + 0:      data = mouse_y >> 8; break;
        case CSR_YPOS + 1:      data = mouse_y & 0xFF; break;
        case CSR_XOFS:			data = mouse_x_offset; break;
        case CSR_YOFS:			data = mouse_y_offset; break;
        case CSR_SCROLL:        { data = mouse_wheel; mouse_wheel = 0; break; }
        case CSR_FLAGS:			data = button_flags; break;
        case CSR_BMP_INDX:		data = bmp_offset; break;
        case CSR_BMP_DATA:		data = csr_data[bmp_offset]; break;
        case CSR_PAL_INDX:		data = m_palette_index; break;
        case CSR_PAL_DATA + 0:  data = (_csr_palette[m_palette_index].color) >> 8; break;
        case CSR_PAL_DATA + 1:  data = (_csr_palette[m_palette_index].color) & 0xFF; break;
    }
    IDevice::write(offset,data);   // update any internal changes too
    return data;
}
void Mouse::write(Word offset, Byte data, bool debug) 
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    switch (offset)
    {
        case CSR_XPOS + 0:      mouse_x = (mouse_x & 0x00ff) | (data << 8); _bCsrIsDirty = true; break;
        case CSR_XPOS + 1:      mouse_x = (mouse_x & 0xff00) | (data << 0); _bCsrIsDirty = true; break;
        case CSR_YPOS + 0:      mouse_y = (mouse_y & 0x00ff) | (data << 8); _bCsrIsDirty = true; break;
        case CSR_YPOS + 1:      mouse_y = (mouse_y & 0xff00) | (data << 0); _bCsrIsDirty = true; break;
        case CSR_XOFS:		    mouse_x_offset = data; _bCsrIsDirty = true;	break;
        case CSR_YOFS:		    mouse_y_offset = data; _bCsrIsDirty = true;	break;
        case CSR_SCROLL:        mouse_wheel = data;     break;
        case CSR_FLAGS:
            if (data & 0x80)    {  _show_SDL_cursor(true);  }
            else                {  _show_SDL_cursor(false); }
            button_flags = data;
            break;
        case CSR_BMP_INDX:	    bmp_offset = data;		break;
        case CSR_BMP_DATA:
            csr_data[bmp_offset] = data;
            break;
        case CSR_PAL_INDX:	m_palette_index = data;	break;
        case CSR_PAL_DATA + 0:
            _csr_palette[m_palette_index].color =
                (_csr_palette[m_palette_index].color & 0x00FF) | (data << 8);
            break;
        case CSR_PAL_DATA + 1:
            _csr_palette[m_palette_index].color =
                (_csr_palette[m_palette_index].color & 0xFF00) | (data << 0);
            break;
    }    

    IDevice::write(offset,data);   // update any internal changes too
}





Word Mouse::OnAttach(Word nextAddr) 
{
    // printf("%s::OnAttach()\n", Name().c_str());    
    Word old_addr = nextAddr;

    DisplayEnum("", 0, "");
    DisplayEnum("", 0, "Mouse Cursor Hardware Registers:");
    DisplayEnum("CSR_BEGIN", nextAddr, " Start of Mouse Cursor Hardware Registers");
    DisplayEnum("CSR_XPOS", nextAddr, " (Word) horizontal mouse cursor coordinate");
    nextAddr += 2;
    DisplayEnum("CSR_YPOS", nextAddr, " (Word) vertical mouse cursor coordinate");
    nextAddr += 2;
    DisplayEnum("CSR_XOFS", nextAddr, " (Byte) horizontal mouse cursor offset");
    nextAddr += 1;
    DisplayEnum("CSR_YOFS", nextAddr, " (Byte) vertical mouse cursor offset");
    nextAddr += 1;
    DisplayEnum("CSR_SCROLL", nextAddr, " (Signed) MouseWheel Scroll: -1, 0, 1");
    nextAddr += 1;
    DisplayEnum("CSR_FLAGS", nextAddr, " (Byte) mouse button flags:");
    DisplayEnum("", 0, " CSR_FLAGS:");
    DisplayEnum("", 0, "      bits 0-4: button states");
    DisplayEnum("", 0, "      bits 5-6: number of clicks");
    DisplayEnum("", 0, "      bits 7:   cursor enable");
    nextAddr += 1;
    DisplayEnum("CSR_BMP_INDX", nextAddr, " (Byte) mouse cursor bitmap pixel offset");
    nextAddr += 1;
    DisplayEnum("CSR_BMP_DATA", nextAddr, " (Byte) mouse cursor bitmap pixel index color");
    nextAddr += 1;
    DisplayEnum("CSR_PAL_INDX", nextAddr, " (Byte) mouse cursor color palette index (0-15)");
    nextAddr += 1;
    DisplayEnum("CSR_PAL_DATA", nextAddr, " (Word) mouse cursor color palette data RGBA4444");
    nextAddr += 2;
    DisplayEnum("CSR_END", nextAddr, "End Mouse Registers");
    DisplayEnum("", 0, "");

    return nextAddr - old_addr;

}

void Mouse::_show_SDL_cursor(bool b)
{
    if (b) 
    {
        button_flags |= 0x80;           // show the SDL cursor
        SDL_ShowCursor(SDL_DISABLE);    // hide the hardware cursor
    } else {
        button_flags &= ~0x80;          // hide the SDL cursor
        SDL_ShowCursor(SDL_ENABLE);     // show the hardware cursor
    }
}
void Mouse::OnInit() 
{
    // printf("%s::OnInit()\n", Name().c_str());    

    _show_SDL_cursor(ENABLE_SDL_MOUSE_CURSOR);
    _render_csr_buffer();
    // copy the rest of the palette from Gfx into the local one
    Gfx* gfx = Bus::GetGfx();
    _csr_palette.clear();
    for (int i=0;i<256;i++)
    {
        PALETTE clr = { 0x0000 };
        clr.a = gfx->alf(i);
        clr.r = gfx->red(i);
        clr.g = gfx->grn(i);
        clr.b = gfx->blu(i);
        _csr_palette.push_back(clr);
    }
}
void Mouse::OnQuit() 
{
    // printf("%s::OnQuit()\n", Name().c_str());    
}

void Mouse::OnActivate()
{
    Gfx* gfx = Bus::GetGfx();

    // create the cursor texture
    if (_cursor_texture == nullptr)
    {
        _cursor_texture = SDL_CreateTexture(gfx->sdl_renderer, SDL_PIXELFORMAT_ARGB4444,
            SDL_TEXTUREACCESS_STREAMING, gfx->res_width, gfx->res_height);       
        if (!_cursor_texture)
            Bus::Error("Error Creating _cursor_texture");     
        else
        {
            SDL_SetTextureBlendMode(_cursor_texture, SDL_BLENDMODE_BLEND);
            SDL_SetRenderTarget(gfx->sdl_renderer, _cursor_texture);
            SDL_SetRenderDrawColor(gfx->sdl_renderer, 0, 0, 0, 0x00);
            SDL_RenderClear(gfx->sdl_renderer);            
        }                       
    }
}

void Mouse::OnDeactivate()
{
    if (_cursor_texture)
    {
        SDL_DestroyTexture(_cursor_texture);
        _cursor_texture = nullptr;
    }
}



void Mouse::OnEvent(SDL_Event* evnt) 
{
    // printf("%s::OnEvent()\n", Name().c_str());    
    
    switch (evnt->type)
    {
        case SDL_WINDOWEVENT:
        {
            switch (evnt->window.event)
            {                
                case SDL_WINDOWEVENT_LEAVE:
                    if (evnt->window.windowID == Gfx::GetWindowID())
                    {   // leaving the main window
                        // printf("EVENT: leaving the main window\n");
                    }
                    else if (evnt->window.windowID == Debug::GetWindowID())
                    {   // leaving the debug window
                        Bus::Write(CSR_FLAGS, Bus::Read(CSR_FLAGS));  // restore prior cursor visibility state
                    }
                    break;
                case SDL_WINDOWEVENT_ENTER:
                    if (evnt->window.windowID == Gfx::GetWindowID())
                    {   // entering the main window
                        // printf("EVENT: entering the main window\n");
                    }
                    else if (evnt->window.windowID == Debug::GetWindowID())
                    {   // entering the debug window
                        SDL_ShowCursor(true);
                    }
                    break;
            } // END switch (SDL_WINDOWEVENT)
            break;
        } // END case SDL_WINDOWEVENT       
        case SDL_MOUSEMOTION:
        {
            if (evnt->window.windowID == Gfx::GetWindowID())
            {
                Gfx* gfx = Bus::GetGfx();
                // trim mouse coordinates to clipping region
                int ww, wh;
                SDL_GetWindowSize(Gfx::GetSDLWindow(), &ww, &wh);
                float aspect = ((float)gfx->GetTimingWidth()/(float)gfx->GetTimingHeight());
                float fh = (float)wh;
                float fw = fh * aspect;    // m_gfx->_aspect;
                if (fw > ww)
                {
                    fw = (float)ww;
                    fh = fw / aspect;
                }
                SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
                float w_aspect = (float)dest.w / (float)gfx->res_width;     // DEBUG_WIDTH;
                float h_aspect = (float)dest.h / (float)gfx->res_height;    // DEBUG_HEIGHT;
                int mx = int((evnt->button.x / w_aspect) - (dest.x / w_aspect));
                int my = int((evnt->button.y / h_aspect) - (dest.y / h_aspect));

                // update the registers
                write_word(CSR_XPOS, mx);
                write_word(CSR_YPOS, my);
            }
            break;
        } // END case SDL_MOUSEMOTION
        case SDL_MOUSEWHEEL:
        {
            if (evnt->window.windowID == Gfx::GetWindowID())
                Bus::Write(CSR_SCROLL, evnt->wheel.y);
            break;
        } // END case SDL_MOUSEWHEEL       

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
            //  CSR_FLAGS:
            //       bits 0-4: button states
            //       bits 5-6: number of clicks
            //       bits 7:   cursor enable   

            // update the button flags
            static Byte s_buttons = 0;
            Byte data = Bus::Read(CSR_FLAGS);
            data &= 0x80;       // mask out all but the cursor enable bit
            int button_mask = (1 << ((evnt->button.button % 7) - 1));
            // clear the button bit
            if (evnt->type == SDL_MOUSEBUTTONUP)
                s_buttons &= ~button_mask;
            // set the button bit
            if (evnt->type == SDL_MOUSEBUTTONDOWN)
            {
                s_buttons |= button_mask;
                // update the number of clicks
                data |= (evnt->button.clicks & 0x03) << 5;
                // update the temporary register data
                data |= s_buttons;
            }
            // finally update the hardware register
            Bus::Write(CSR_FLAGS, data);
            break;
        } // END Mouse Button States
    }
}

void Mouse::OnUpdate(float fElapsedTime) 
{
    // printf("%s::OnUpdate()\n", Name().c_str());    
    _display_SDL_cursor();
}

void Mouse::OnRender() 
{
    // printf("%s::OnRender()\n", Name().c_str());    
    Gfx* gfx = Bus::GetGfx();

    // if (_bCsrIsVisible == false)
    //     return;

    int ww, wh;
    SDL_GetWindowSize(Gfx::GetSDLWindow(), &ww, &wh);
    float aspect = ((float)gfx->GetTimingWidth()/(float)gfx->GetTimingHeight());
    float fh = (float)wh;
    float fw = fh * aspect;
    if (fw > ww)
    {
        fw = (float)ww;
        fh = fw / aspect;
    }
    SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
    SDL_SetRenderTarget(gfx->sdl_renderer, gfx->sdl_target_texture);
    SDL_RenderCopy(gfx->sdl_renderer, _cursor_texture, NULL, &dest);        
}

void Mouse::_display_SDL_cursor()
{
    Gfx* gfx = Bus::GetGfx();    
    void *pixels;
    int pitch;
    if (SDL_LockTexture(_cursor_texture, NULL, &pixels, &pitch) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError());
        Bus::Error("");
    }
    else
    {
        static bool b_wasCleared = false;
        if (b_wasCleared == false)
        {
            // start with a clear texture (pixel streaming version)
            for (int ty=0; ty<gfx->res_height; ty++)
                for (int tx=0; tx<gfx->res_width; tx++)
                    gfx->_setPixel_unlocked(pixels, pitch, tx, ty, 0, true);    
            b_wasCleared = true;
        }
        if (read(CSR_FLAGS) & 0x80)
        {
            b_wasCleared = false;
            // render the cursor
            int x = (Sint16)Bus::Read_Word(CSR_XPOS) + (Sint8)Bus::Read(CSR_XOFS);
            int y = (Sint16)Bus::Read_Word(CSR_YPOS) + (Sint8)Bus::Read(CSR_YOFS);   

            int id = 0;
            for (int v=0; v<16; v++)
            {
                for (int h=0; h<16; h++)
                {
                    Byte c = csr_data[id++];
                    _setPixel_cursor(pixels, pitch, h+x, v+y, c);
                }
            }
        }
        SDL_UnlockTexture(_cursor_texture); 
    }   
}

void Mouse::_render_csr_buffer()
{
    // renders the user ledgible data from the csr_buffer to the csr_data vector
    csr_data.clear();
    for (auto& li : csr_buffer)    
    {
        for (auto& ch : li)
        {
            Byte c = 0xff;  // space is 100% transparent, use character index 255.
            if (ch != ' ')
            {
                if (ch>='0' && ch<='9')
                    c = ch-'0';
                else if (ch>='a' && ch<='f')
                    c = (ch-'a')+10;
                else if (ch>='A' && ch<='F')
                    c = (ch-'A')+10;
            }
            csr_data.push_back(c);
        }
    }
}

void Mouse::_setPixel_cursor(void* pixels, int pitch, int x, int y, Byte color_index)
{    
    Gfx* gfx = Bus::GetGfx();
    // strict bounds checking
    if (x<0 || y<0)         return;
    if (x>=gfx->res_width)  return;
    if (y>=gfx->res_height) return;

    Uint16 *dst = (Uint16*)((Uint8*)pixels + (y * pitch) + (x*sizeof(Uint16)));
    Uint16 pixel = *dst;	// 0xARGB

    constexpr bool use_alpha = true;    // testing:  this flag does work as expected.
                                        //      Update the _setPixel_unlocked() method in Gfx.
    if (use_alpha)
    {
        Byte r1 = (pixel & 0x0f00) >> 8;
        Byte g1 = (pixel & 0x00f0) >> 4;
        Byte b1 = (pixel & 0x000f) >> 0;
        
        Byte a2 = this->alf(color_index);
        Byte r2 = this->red(color_index);
        Byte g2 = this->grn(color_index);
        Byte b2 = this->blu(color_index);

        Byte r = (((r1 * (16-a2))) + (r2 * (a2+1))) >> 4;
        Byte g = (((g1 * (16-a2))) + (g2 * (a2+1))) >> 4;
        Byte b = (((b1 * (16-a2))) + (b2 * (a2+1))) >> 4;

        if (alf(color_index) != 0)
        {
            *dst = (
                (a2<<12) | 
                (r<<8) | 
                (g<<4) | 
                (b)
            );          
        }	
    }
    else
    {
        *dst = 
        (
            0xF000 |                    // full alpha, no color mixing
            (red(color_index)<<8) |
            (grn(color_index)<<4) |
            blu(color_index)
        );    
       
    }
}

