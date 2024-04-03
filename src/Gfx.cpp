// *************************************************
// *
// * Gfx.cpp
// *
// ***********************************

#include <sstream>
#include "Gfx.hpp"
#include "GfxCore.hpp"
#include "Bus.hpp"
#include "font8x8_system.hpp"

Byte Gfx::read(Word offset, bool debug)
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);
    switch (offset)
    {
        case GFX_MODE:         
        {   
            data = s_gfx_mode; 
            break;
        }
        case GFX_EMU:           data = s_gfx_emu; break;
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
    
    GfxCore* gfx_core = Bus::GetGfxCore();

    switch (offset)
    {
        case GFX_MODE:
            if (data != s_gfx_mode)
            {
                // ToDo: Only change data if valid GMODE
                if (gfx_core->VerifyGmode(data))
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
			Bus::IsDirty(true);
			break;
		}
		case GFX_PAL_CLR+0:	{
			Word c = _palette[_gfx_pal_idx].color & 0x00ff;
			_palette[_gfx_pal_idx].color = c | ((Word)data << 8);
			data = _palette[_gfx_pal_idx].color;
			Bus::IsDirty(true);
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
    DisplayEnum("GFX_MODE", nextAddr, "(Byte) Graphics Mode");
	DisplayEnum("", 0, "\t     - bit 0-4   = Resolution Modes 0-31");
	DisplayEnum("", 0, "\t     - bit 7     = 0:text,  1:bitmap");
    DisplayEnum("", 0, "");
    nextAddr++;

    DisplayEnum("GFX_EMU", nextAddr, "(Byte) Emulation Flags");
	DisplayEnum("", 0, "\t     - bits 0-2  = Active Monitor 0-7");
	DisplayEnum("", 0, "\t     - bits 3-5  = reserved");
	DisplayEnum("", 0, "\t     - bit  6    = 0:vsync off, 1:vsync on");
	DisplayEnum("", 0, "\t     - bit  7    = 0:windowed, 1:fullscreen");
    DisplayEnum("", 0, "");
    nextAddr++;

    DisplayEnum("GFX_PAL_IDX", nextAddr, " (Byte) Color Palette Index");
    DisplayEnum("", 0, "GFX_PAL_IDX: 0-255");
    DisplayEnum("", 0, "Note: Use this register to set the index into theColor Palette. ");
    DisplayEnum("", 0, "  Set this value prior referencing color data (GFX_PAL_CLR).");
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


    return nextAddr - old_addr;
}

void Gfx::OnInit()
{
    // printf("%s::OnInit()\n", Name().c_str());

    // _init_tests();

    // INITIALIZE PALETTE DATA
    if (_palette.size() == 0)
    {
        // BASIC COLORS (0-15) STANDARD CGA COLORS
        std::vector<PALETTE> ref = {    
			{ 0xF000 },		// 0: black
			{ 0xF555 },		// 1: dk gray
			{ 0xF007 },		// 2: dk blue
			{ 0xF600 },		// 3: dk red
			{ 0xF140 },		// 4: dk green
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
                // printf("%3d:$%04X\n", (int)_palette.size(), ent.color);
            }
        }
    }

    // initialize the font glyph buffer
    for (int i=0; i<256; i++)
        for (int r=0; r<8; r++)
            _gfx_glyph_data[i][r] = font8x8_system[i][r];
}

void Gfx::_init_tests()
{
    // Bus::Write(GFX_PAL_IDX, 0x12);
    // printf("GFX_PAL_IDX: $%02X\n", _gfx_pal_idx);
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





