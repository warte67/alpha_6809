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
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    switch (offset)
    {
        // system registers
		case SYS_STATE: {
			Byte err = C6809::s_sys_state & 0xF0;
			C6809::s_sys_state &= 0x0F;
			data = C6809::s_sys_state | err; 
			break;
		}        
		case SYS_SPEED + 0:		data = Bus::_sys_cpu_speed >> 8; break;
		case SYS_SPEED + 1:		data = Bus::_sys_cpu_speed & 0xFF; break;
		case SYS_CLOCK_DIV:		data = Bus::_clock_div; break;
		case SYS_TIMER + 0:		data = Bus::_clock_timer >> 8; break;
		case SYS_TIMER + 1:		data = Bus::_clock_timer & 0xff; break;

        // debug registers
        case DBG_BRK_ADDR + 0: data = reg_brk_addr >> 8;   break;
        case DBG_BRK_ADDR + 1: data = reg_brk_addr & 0xFF; break;
        case DBG_FLAGS: {
            (s_bIsDebugActive) ? reg_flags |= 0x80 : reg_flags &= ~0x80; // Enable
            (s_bSingleStep)     ? reg_flags |= 0x40 : reg_flags &= ~0x40; // Single-Step
            reg_flags &= ~0x20;     // zero for Clear all Breakpoints
            (mapBreakpoints[reg_brk_addr]) ? reg_flags |= 0x10 : reg_flags &= ~0x10;
            reg_flags &= ~0x08;     // FIRQ
            reg_flags &= ~0x04;     // IRQ
            reg_flags &= ~0x02;     // NMI
            reg_flags &= ~0x01;     // RESET

            data = reg_flags; 
            break;
        }
    }

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}
void Debug::write(Word offset, Byte data, bool debug) 
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    switch (offset)
    {
        // system registers
		case SYS_STATE: { 
			C6809::s_sys_state = data;
			break;
		}        
		case SYS_TIMER + 0: {
			Bus::_clock_timer = (data << 8) | (Bus::_clock_timer & 0x0f);
			break;
		}
		case SYS_TIMER + 1: {
			Bus::_clock_timer = (data << 0) | (Bus::_clock_timer & 0xf0);
			break;
		}

        // debug registers
        case DBG_BRK_ADDR + 0: reg_brk_addr = (reg_brk_addr & 0x00ff) | (data << 8); break;
        case DBG_BRK_ADDR + 1: reg_brk_addr = (reg_brk_addr & 0xff00) | (data << 0); break;
        case DBG_FLAGS: {
            reg_flags = data;
            (reg_flags & 0x80) ? s_bIsDebugActive = true : s_bIsDebugActive = false;
            (reg_flags & 0x40) ? s_bSingleStep = true : s_bSingleStep = false;
            if (reg_flags & 0x20)  cbClearBreaks();
            (reg_flags & 0x10) ? mapBreakpoints[reg_brk_addr] = true : mapBreakpoints[reg_brk_addr] = false;
            if (reg_flags & 0x08)   cbFIRQ();
            if (reg_flags & 0x04)   cbIRQ();
            if (reg_flags & 0x02)   cbNMI();
            if (reg_flags & 0x01)   cbReset();
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
    
    DisplayEnum("", 0, "");
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
    Byte at=0xf0;
    for (auto& c : _db_bfr)
    {
        c.chr = ' ';
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

    // handle debug events for all windows
    switch(evnt->type)
    {
        case SDL_KEYDOWN: 
        {
            // ****************************
            // * Debugger Specific Events *
            // ****************************

            SDL_Keymod km = SDL_GetModState();
            if (km & KMOD_ALT)
            {
                if (evnt->key.keysym.sym == SDLK_d)
                {
                    // _bIsDebugActive = !_bIsDebugActive;
                    Byte data = Bus::Read(DBG_FLAGS);
                    data ^= 0x80;
                    Bus::Write(DBG_FLAGS, data);
                    bIsCursorVisible = false;
                    bMouseWheelActive = false;

                    if (s_bIsDebugActive)
                        SDL_ShowWindow(Debug::GetSDLWindow());
                    else
                        SDL_HideWindow(Debug::GetSDLWindow());
                    
                    // if (s_bIsDebugActive) // enable the cursor during debug
                    // {
                    //     Byte data = Bus::Read(CSR_FLAGS);
                    //     data |= 0x20;
                    //     Bus::Write(CSR_FLAGS, data);
                    // }
                }
                if (evnt->key.keysym.sym == SDLK_r)
                {
                    s_bSingleStep = !s_bSingleStep;
                    bMouseWheelActive = false;
                }
            }
            // [F10] or [SPACE] == Step Over
            if (evnt->key.keysym.sym == SDLK_F10 || evnt->key.keysym.sym == SDLK_SPACE)
            {
                cbStepOver();
            }    
            // [F11] == Step Into
            if (evnt->key.keysym.sym == SDLK_F11)
            {
                cbStepIn();
            }                
            break;           
        }
    }

    // if not a debug event, just return now
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
                    //printf("%s::OnEvent(): SDL_WINDOWEVENT_CLOSE\n", Name().c_str());
                    s_bIsDebugActive = false;
                    // SDL_HideWindow(sdl_debug_window);
                    SDL_MinimizeWindow(sdl_debug_window);   // just minimize instead of close
                    break;
                }                
                case SDL_WINDOWEVENT_MINIMIZED:
                {
                    //printf("%s::OnEvent(): SDL_WINDOWEVENT_MINIMIZED\n", Name().c_str());
                    s_bIsDebugActive = false;
                    break;
                }
                case SDL_WINDOWEVENT_RESTORED:
                {
                    //printf("%s::OnEvent(): SDL_WINDOWEVENT_RESTORED\n", Name().c_str());
                    s_bIsDebugActive = true;
                    break;
                }
                case SDL_WINDOWEVENT_ENTER:
                {
                    //printf("%s::OnEvent(): SDL_WINDOWEVENT_ENTER\n", Name().c_str());
                    bIsMouseOver = true;
                    break;
                }
                case SDL_WINDOWEVENT_LEAVE:
                {
                    //printf("%s::OnEvent(): SDL_WINDOWEVENT_LEAVE\n", Name().c_str());
                    bIsMouseOver = false;
                    break;
                }
                case SDL_WINDOWEVENT_FOCUS_LOST:
                {
                    //printf("%s::OnEvent(): SDL_WINDOWEVENT_FOCUS_LOST\n", Name().c_str());
                    bIsCursorVisible = false;
                    break;
                }                
            }
            break;
        }

        case SDL_KEYDOWN: 
        {
            // ****************************
            // * Debugger Specific Events *
            // ****************************

            // perform debug enabled specific events
            if (s_bIsDebugActive)
            {
                if (evnt->key.keysym.sym == SDLK_ESCAPE)
                    bIsCursorVisible = false;
                if (bIsCursorVisible)
                {
                    if (evnt->key.keysym.sym == SDLK_LEFT || evnt->key.keysym.sym == SDLK_BACKSPACE)
                        if (csr_x > 1)
                            while (!CoordIsValid(--csr_x, csr_y));
                    if (evnt->key.keysym.sym == SDLK_RIGHT)
                        if (csr_x < 28)
                            while (!CoordIsValid(++csr_x, csr_y));
                    if (evnt->key.keysym.sym == SDLK_UP)
                    {
                        if (csr_y == 1)			mem_bank[0] -= 8;
                        else if (csr_y == 11)	mem_bank[1] -= 8;
                        else if (csr_y == 21)	mem_bank[2] -= 8;
                        else if (csr_y > 1)		while (!CoordIsValid(csr_x, --csr_y));
                    }
                    if (evnt->key.keysym.sym == SDLK_DOWN)
                    {
                        if (csr_y == 9)			mem_bank[0] += 8;
                        else if (csr_y == 19)	mem_bank[1] += 8;
                        else if (csr_y == 29)	mem_bank[2] += 8;
                        else if (csr_y < 30)	while (!CoordIsValid(csr_x, ++csr_y));
                    }
                    if (evnt->key.keysym.sym == SDLK_RETURN)
                        bIsCursorVisible = false;
                }          
            }
            break;
        } // SDL_KEYDOWN

        case SDL_MOUSEWHEEL:
            mouse_wheel = evnt->wheel.y;
            break;
    }
}

void Debug::OnUpdate(float fElapsedTime) 
{
    // printf("%s::OnUpdate()\n", Name().c_str());   

    if (Bus::GetC6809() == nullptr)     return;
    if (!IsActive())    return;

    if (bIsCursorVisible)
        SDL_RaiseWindow(Debug::GetSDLWindow());

    // frame rate limit the debug layer updates
    const float delay = 1.0f / 30.0f;
    static float delayAcc = fElapsedTime;
    delayAcc += fElapsedTime;
    if (delayAcc >= delay)
    {
        delayAcc -= delay;

        // clear the text buffer
        for (auto &c : _db_bfr)
        {
            c.attr = 0xf0;
            c.chr = ' ';
        }

        // call update functions
        MouseStuff();
        KeyboardStuff();

        DumpMemory(1,  1, mem_bank[0]);
        DumpMemory(1, 11, mem_bank[1]);
        DumpMemory(1, 21, mem_bank[2]);

        DrawCpu(39, 1);
        DrawCode(39, 6);

        DrawButtons();    
        HandleButtons();
        DrawBreakpoints();
        if (!EditRegister(fElapsedTime))
            DrawCursor(fElapsedTime);

        // instruction text
        OutText(1, 31, "[SPACE]    Step", 0x80);
        OutText(1, 32, "[ALT-X]    Quit", 0x80);
        OutText(1, 33, "[ALT-D] ~ Debug", 0x80);
        OutText(1, 34, "[ALT-R] RunStop", 0x80);
        OutText(1, 35, "[ALT-ENTER] Toggles between Fullscreen and Windowed", 0x80);

    }

    // present the debug screen data
    _updateDebugScreen(); 
}

bool Debug::EditRegister(float fElapsedTime)
{
    if (nRegisterBeingEdited.reg == EDIT_REGISTER::EDIT_NONE)
        return false;

    C6809* cpu = Bus::GetC6809();

    static float delay = 0.0625f;
    static float delayAcc = fElapsedTime;
    static int ci = 9;
    delayAcc += fElapsedTime;
    std::string ch = " ";

    if (delayAcc > delay + fElapsedTime)
    {
        delayAcc -= delay;
        ci++;
        if (ci > 15)	ci = 9;
    }
    // OutGlyph(csr_x, csr_y, 0x8f, (rand() % 16)<<4);

    Word data = 0;
    switch (nRegisterBeingEdited.reg) {
        case EDIT_CC:	data = (Word)cpu->getCC() << 8; break;
        case EDIT_D:	data = cpu->getD(); break;
        case EDIT_A:	data = (Word)cpu->getA() << 8; break;
        case EDIT_B:	data = (Word)cpu->getB() << 8; break;
        case EDIT_X:	data = cpu->getX(); break;
        case EDIT_Y:	data = cpu->getY(); break;
        case EDIT_U:	data = cpu->getU(); break;
        case EDIT_PC:	data = cpu->getPC(); s_bSingleStep = true;  break;
        case EDIT_S:	data = cpu->getS(); break;
        case EDIT_DP:	data = (Word)cpu->getDP() << 8; break;
        case EDIT_BREAK: data = new_breakpoint; break;
        case EDIT_NONE: break;
    }
    Byte digit = csr_x - nRegisterBeingEdited.x_min;
    Byte num = 0;
    if (digit == 0)	num = ((data & 0xf000) >> 12);
    if (digit == 1)	num = ((data & 0x0f00) >> 8);
    if (digit == 2)	num = ((data & 0x00f0) >> 4);
    if (digit == 3)	num = ((data & 0x000f) >> 0);
    ch = _hex(num, 1);
    // OutGlyph(csr_x, csr_y, ch[0], 0);
    OutGlyph(csr_x, csr_y, ch[0], (rand() % 16)<<0);

    // respond to numeric keys
    SDL_Keycode hx[] = { SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
                         SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,
                         SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
                         SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F };
    // char k[] = { '0', '1', '2', '3', '4', '5', '6', '7',
    //              '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    char d[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static bool state[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    const Uint8* keybfr = SDL_GetKeyboardState(NULL);
    Word n = 0;
    bool bKeyPressed = false;
    for (int t = 0; t < 16; t++)
    {
        if (state[t] == 0 && keybfr[hx[t]])
        {
            n = d[t];
            bKeyPressed = true;
            state[t] = 1;
        }
        // reset the key
        if (state[t] == 1 && !keybfr[hx[t]])	state[t] = 0;
    }

    if (bKeyPressed)
    {
        //printf("pressed\n");
        if (digit == 0)		data = (data & 0x0fff) | (n << 12);
        if (digit == 1)		data = (data & 0xf0ff) | (n << 8);
        if (digit == 2)		data = (data & 0xff0f) | (n << 4);
        if (digit == 3)		data = (data & 0xfff0) | (n << 0);

        switch (nRegisterBeingEdited.reg) {
            case EDIT_CC:	cpu->setCC(data >> 8);  break;
            case EDIT_D:	cpu->setD(data);		break;
            case EDIT_A:	cpu->setA(data >> 8);	break;
            case EDIT_B:	cpu->setB(data >> 8);	break;
            case EDIT_X:	cpu->setX(data);		break;
            case EDIT_Y:	cpu->setY(data);		break;
            case EDIT_U:	cpu->setU(data);		break;
            case EDIT_PC:	cpu->setPC(data);		bMouseWheelActive = false;  break;
            case EDIT_S:	cpu->setS(data);		break;
            case EDIT_DP:	cpu->setDP(data >> 8);  break;
            case EDIT_BREAK: new_breakpoint = data; break;
            case EDIT_NONE: break;
        }
        if (csr_x < nRegisterBeingEdited.x_max)
            csr_x++;
    }
    SDL_Keycode ex[] = { SDL_SCANCODE_LEFT , SDL_SCANCODE_RIGHT , SDL_SCANCODE_RETURN , SDL_SCANCODE_ESCAPE };
    static bool bEx[] = { 0, 0, 0, 0 };
    for (int t = 0; t < 4; t++)
    {
        if (bEx[t] == 0 && keybfr[ex[t]])
        {
            // left
            if (ex[t] == SDL_SCANCODE_LEFT)
                if (csr_x > nRegisterBeingEdited.x_min)
                    csr_x--;
            // right
            if (ex[t] == SDL_SCANCODE_RIGHT)
                if (csr_x < nRegisterBeingEdited.x_max)
                    csr_x++;
            // enter updates and saves the register
            if (ex[t] == SDL_SCANCODE_RETURN || ex[t] == SDL_SCANCODE_ESCAPE)
            {
                if (nRegisterBeingEdited.reg == EDIT_REGISTER::EDIT_BREAK)
                {
                    mapBreakpoints[new_breakpoint] = true;
                    nRegisterBeingEdited.reg = EDIT_REGISTER::EDIT_NONE;
                    bEditingBreakpoint = false;
                }
                nRegisterBeingEdited.reg = EDIT_REGISTER::EDIT_NONE;
            }
            bEx[t] = 1;
        }
        else if (bEx[t] == 1 && !keybfr[ex[t]])
            bEx[t] = 0;
    }
    return true;
}

void Debug::DrawCursor(float fElapsedTime)
{
    if (!bIsCursorVisible)	return;
    
    // C6809* cpu = Bus::GetC6809();
    std::string ch = " ";

    // which character is under the cursor?
    switch (csr_at)
    {
        case CSR_AT::CSR_AT_ADDRESS:
        {
            Word addr = 0;
            if (csr_y < 10) addr = mem_bank[0] + ((csr_y - 1) * 8);
            else if (csr_y < 20) addr = mem_bank[1] + ((csr_y - 11) * 8);
            else if (csr_y < 30) addr = mem_bank[2] + ((csr_y - 21) * 8);
            Byte digit = csr_x - 1;
            Byte num = 0;
            if (digit == 0)	num = ((addr & 0xf000) >> 12);
            if (digit == 1)	num = ((addr & 0x0f00) >> 8);
            if (digit == 2)	num = ((addr & 0x00f0) >> 4);
            if (digit == 3)	num = ((addr & 0x000f) >> 0);
            ch = _hex(num, 1);
            break;
        }
        case CSR_AT::CSR_AT_DATA:
        {
            Byte ofs = ((csr_x - 5) / 3) + ((csr_y - 1) * 8);
            Byte digit = ((csr_x + 1) % 3) - 1;
            Byte num = 0;
            Word addr = mem_bank[0];
            if (csr_y > 10 && csr_y < 20) { ofs -= 80; addr = mem_bank[1]; }

            if (csr_y > 20) { ofs -= 160; addr = mem_bank[2]; }

            Byte data = Bus::Read(addr + ofs, true);
            if (digit == 0) num = (data & 0xf0) >> 4;
            if (digit == 1) num = (data & 0x0f) >> 0;
            ch = _hex(num, 1);
            break;
        }
    }
    // display the reversed character
    OutGlyph(csr_x, csr_y, ch[0], (rand() % 16)<<0);
}



void Debug::DrawBreakpoints()
{
    // C6809* cpu = Bus::GetC6809();

    int x = 56, y = 33;		// y <= 38
    // Uint8 ci = 0x0C;

    // build a vector of active breakpoints
    std::vector<Word> breakpoints;
    for (auto& bp : mapBreakpoints)
        if (bp.second)
            breakpoints.push_back(bp.first);
    // standard display
    if (breakpoints.size() < 8)
    {
        for (Word t = 0; t < breakpoints.size(); t++)
        {
            std::string strBkpt = "[$";
            strBkpt += _hex(breakpoints[t], 4);
            strBkpt += "]";
            OutText(x, y, strBkpt, 0xA0);
            y++;
        }
    }
    // oversized, mousewheel scrollable, display
    else
    {
        if (mw_brk_offset < 0)							mw_brk_offset = 0;
        if ((unsigned)mw_brk_offset + 7 > breakpoints.size())		mw_brk_offset = breakpoints.size() - 7;

        int index = mw_brk_offset;
        for (int t = 0; t < 7; t++)
        {
            std::string strBkpt;

            if (t == 0 && mw_brk_offset > 0)
                strBkpt = "[ ... ]";
            else if (t == 6 && (unsigned)index != breakpoints.size() - 1)
                strBkpt = "[ ... ]";
            else
            {
                strBkpt = "[$";
                strBkpt += _hex(breakpoints[index], 4);
                strBkpt += "]";
            }
            OutText(x, y, strBkpt, 0xA0);
            y++;
            index++;
        }
    }
}


void Debug::HandleButtons()
{
    // C6809* cpu = Bus::GetC6809();
    int mx, my;
    _correctMouseCoords(mx, my);
    Uint32 btns = SDL_GetRelativeMouseState(NULL, NULL);

    static bool last_LMB = false;
    if (btns & 1 && !last_LMB)
    {
        for (auto& a : vButton)
        {
            if (my == a.y_pos && mx >= a.x_min && mx <= a.x_max)
            {
                if (a.cbFunction)	(this->*a.cbFunction)();
            }
        }
    }
    last_LMB = (btns & 1);
}


void Debug::DrawButtons()
{
    // C6809* cpu = Bus::GetC6809();

    // change the run/stop according to the single step state
    if (s_bSingleStep)
    {
        vButton[5].text = " RUN!";
        vButton[5].x_min = 17;
        vButton[5].clr_index = 0xB;
    }
    else {
        vButton[5].text = " STOP";
        vButton[5].x_min = 17;
        vButton[5].clr_index = 0xA;
    }
    if (bEditingBreakpoint)
    {
        vButton[9].text = "[$";
        vButton[9].text += _hex(new_breakpoint, 4);
        vButton[9].text += "]";
        vButton[9].x_min = 48;
        vButton[9].clr_index = 0xC;
    }
    else
    {
        vButton[9].text = "ADD BRK";
        vButton[9].x_min = 48;
        vButton[9].clr_index = 0xC;
    }
    // draw the buttons
    for (auto& a : vButton)
    {
        int x1 = a.x_min;
        int x2 = a.x_max;
        int y = a.y_pos;
        // background
        int p = (a.clr_index % 16) | ((a.clr_index % 16)<<4);
        for (int x = x1; x <= x2; x++)
        {
            OutGlyph(x, y, 0x8f, p);
        }
        // foreground
        int sX = x1 + ((x2 - x1) / 2) - (int)a.text.size() / 2;
        OutText(sX, y, a.text, a.clr_index % 16);
    }
}


void Debug::DrawCode(int col, int row)
{
    C6809* cpu = Bus::GetC6809();

    std::string code = "";
    int line = 0;
    Word next = 0;
    sDisplayedAsm.clear();

    if (bMouseWheelActive)
    {
        Word cpu_PC = cpu->getPC();
        Word offset = cpu_PC + mousewheel_offset;
        while (line < 24)
        {
            if (offset < cpu_PC)
            {
                bool atBreak = false;
                if (mapBreakpoints[offset])	atBreak = true;
                sDisplayedAsm.push_back(offset);
                code = cpu->disasm(offset, offset);
                if (atBreak)
                    OutText(col, row + line, code, 0xA0);    // red
                else
                    OutText(col, row + line, code, 0x40);    // dk green
                line++;
            }
            if (offset == cpu_PC)
            {
                bool atBreak = false;
                if (mapBreakpoints[offset])	atBreak = true;
                sDisplayedAsm.push_back(offset);
                code = cpu->disasm(offset, offset);
                if (atBreak)
                    OutText(col, row + line, code, 0x50);  // brown
                else
                    OutText(col, row + line, code, 0xC0);    // yellow
                line++;
            }
            if (offset > cpu_PC)
            {
                bool atBreak = false;
                if (mapBreakpoints[offset])	atBreak = true;
                sDisplayedAsm.push_back(offset);
                code = cpu->disasm(offset, offset);
                if (atBreak)
                    OutText(col, row + line, code, 0x30);   // dk red
                else
                    OutText(col, row + line, code, 0xB0);   // lt green
                line++;
            }

        }

    }
    else
    {
        // draw the last several lines
        for (auto& a : asmHistory)
        {
            if (a != cpu->getPC())
            {
                bool atBreak = false;
                if (mapBreakpoints[a])	atBreak = true;
                sDisplayedAsm.push_back(a);
                code = cpu->disasm(a, next);
                if (atBreak)
                    OutText(col, row + line++, code, 0x30);      // 0x30 dk red
                else
                    OutText(col, row + line++, code, 0x10);    // 0x10 DK gray
            }
        }
        // draw the current line
        sDisplayedAsm.push_back(cpu->getPC());
        code = cpu->disasm(cpu->getPC(), next);
        if (mapBreakpoints[cpu->getPC()])
            OutText(col, row + line++, code, 0xA0);              // 0xA0 red
        else
            OutText(col, row + line++, code, 0xF0);            // 0xF0 white
        // create a history of addresses to display in the future
        static Word last = cpu->getPC();
        if (last != cpu->getPC())
        {
            last = cpu->getPC();
            asmHistory.push_back(cpu->getPC());
            while (asmHistory.size() > 12)
                asmHistory.pop_front();
        }
        // draw the next several future lines
        while (line < 24)
        {
            bool atBreak = false;
            if (mapBreakpoints[next])	atBreak = true;
            sDisplayedAsm.push_back(next);
            code = cpu->disasm(next, next);
            if (atBreak)
                OutText(col, row + line++, code, 0X30);          // 0X30 DK RED
            else
                OutText(col, row + line++, code,0X80);        // 0X80 LT GRAY
        }
    }
}


void Debug::DrawCpu(int x, int y)
{
    C6809* cpu = Bus::GetC6809();

    int RamX = x, RamY = y;
    // Condition Codes
    RamX += OutText(RamX, RamY, "CC($", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getCC(), 2).c_str(), 0xC0);
    RamX += OutText(RamX, RamY, "): ", 0xB0);
    if (cpu->getCC_E())		RamX += OutText(RamX, RamY, "E", 0xC0);
    else RamX += OutText(RamX, RamY, "e", 0xB0);
    if (cpu->getCC_F())		RamX += OutText(RamX, RamY, "F", 0xC0);
    else RamX += OutText(RamX, RamY, "f", 0xB0);
    if (cpu->getCC_H())		RamX += OutText(RamX, RamY, "H", 0xC0);
    else RamX += OutText(RamX, RamY, "h", 0xB0);
    if (cpu->getCC_I())		RamX += OutText(RamX, RamY, "I", 0xC0);
    else RamX += OutText(RamX, RamY, "i", 0xB0);
    if (cpu->getCC_N())		RamX += OutText(RamX, RamY, "N", 0xC0);
    else RamX += OutText(RamX, RamY, "n", 0xB0);
    if (cpu->getCC_Z())		RamX += OutText(RamX, RamY, "Z", 0xC0);
    else RamX += OutText(RamX, RamY, "z", 0xB0);
    if (cpu->getCC_V())		RamX += OutText(RamX, RamY, "V", 0xC0);
    else RamX += OutText(RamX, RamY, "v", 0xB0);
    if (cpu->getCC_C())		RamX += OutText(RamX, RamY, "C", 0xC0);
    else RamX += OutText(RamX, RamY, "c", 0xB0);
    RamX = x; RamY++;	// carraige return(ish)

    // D = (A<<8) | B & 0x00FF
    RamX += OutText(RamX, RamY, "D:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getD(), 4), 0xC0);
    RamX += OutText(RamX, RamY, " (A:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getA(), 2), 0xC0);
    RamX += OutText(RamX, RamY, " B:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getB(), 2), 0xC0);
    RamX += OutText(RamX, RamY, ")", 0xB0);
    RamX = x; RamY++;	// carraige return(ish)

    // X
    RamX += OutText(RamX, RamY, " X:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getX(), 4), 0xC0);
    // Y
    RamX += OutText(RamX, RamY, " Y:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getY(), 4), 0xC0);
    // U
    RamX += OutText(RamX, RamY, " U:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getU(), 4), 0xC0);
    RamX = x; RamY++;	// carraige return(ish)
    // PC
    RamX += OutText(RamX, RamY, "PC:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getPC(), 4), 0xC0);
    // S
    RamX += OutText(RamX, RamY, " S:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getS(), 4), 0xC0);
    // DP
    RamX += OutText(RamX, RamY, " DP:$", 0xB0);
    RamX += OutText(RamX, RamY, _hex(cpu->getDP(), 2), 0xC0);
    RamX = x; RamY++;	// carraige return(ish)
}


void Debug::OutGlyph(int col, int row, Byte glyph, Byte color_index)
{
    Word index = row*(DEBUG_WIDTH/8) + col;
    _db_bfr[index].chr = glyph;
    _db_bfr[index].attr = color_index;
}

int Debug::OutText(int col, int row, std::string text, Byte color_index)
{
    int pos = 0;
    for (auto& a : text)
    {
        OutGlyph(col++, row, a, color_index);
        pos++;
    }
    return pos;
}

std::string Debug::_hex(Uint32 n, Uint8 d)
{
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
}

void Debug::DumpMemory(int col, int row, Word addr)
{
    const bool use_debug_read = false;
    int line = 0;
    for (int ofs = addr; ofs < addr + 0x48; ofs += 8)
    {
        int c = col;
        std::string out = _hex(ofs, 4) + " ";
        if (use_debug_read)
            for (int b = 0; b < 8; b++)
                out += _hex(Bus::Read(ofs + b, true), 2) + " ";
        else
            for (int b = 0; b < 8; b++)
                out += _hex(Bus::Read(ofs + b), 2) + " ";

        c += OutText(col, row + line, out.c_str(), 0xe0);

        bool characters = true;
        if (characters)
        {
            for (int b = 0; b < 8; b++)
            {
                Byte data;
                if (use_debug_read)
                    data = Bus::Read(ofs + b, true);
                else
                    data = Bus::Read(ofs + b);
                OutGlyph(c++, row + line, data, 0xd0);
            }
        }
        line++;
    }
}

void Debug::KeyboardStuff()
{
    if (!bIsCursorVisible)	return;

    // C6809* cpu = Bus::GetC6809();

    SDL_Keycode hx[] = { SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
                         SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,
                         SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
                         SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F };
    // char k[] = { '0', '1', '2', '3', '4', '5', '6', '7',
    //              '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    char d[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    static bool state[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    const Uint8* keybfr = SDL_GetKeyboardState(NULL);

    // check for valid key presses
    bool bKeyPressed = false;
    Byte ch = 0;
    for (int t = 0; t < 16; t++)
    {
        if (state[t] == 0 && keybfr[hx[t]])
        {
            state[t] = 1;
            bKeyPressed = true;
            ch = d[t];
            // printf("%c\n", k[t]);
        }
        // reset the key
        if (state[t] == 1 && !keybfr[hx[t]])	state[t] = 0;
    }

    // respond to [DEL]
    // ...

    if (!bKeyPressed)	return;

    switch (csr_at)
    {
        case CSR_AT::CSR_AT_ADDRESS:
        {
            Word addr = 0;
            if (csr_y < 10) addr = mem_bank[0] + ((csr_y - 1) * 8);
            else if (csr_y < 20) addr = mem_bank[1] + ((csr_y - 11) * 8);
            else if (csr_y < 30) addr = mem_bank[2] + ((csr_y - 21) * 8);
            Byte digit = csr_x - 1;
            if (digit == 0)	addr = (addr & 0x0fff) | (ch << 12);
            if (digit == 1)	addr = (addr & 0xf0ff) | (ch << 8);
            if (digit == 2)	addr = (addr & 0xff0f) | (ch << 4);
            if (digit == 3)	addr = (addr & 0xfff0) | (ch << 0);
            if (csr_y < 10)			mem_bank[0] = addr - ((csr_y - 1) * 8);
            else if (csr_y < 20)	mem_bank[1] = addr - ((csr_y - 11) * 8);
            else if (csr_y < 30)	mem_bank[2] = addr - ((csr_y - 21) * 8);
            if (csr_x < 5)	while (!CoordIsValid(++csr_x, csr_y));
            break;
        }
        case CSR_AT::CSR_AT_DATA:
        {
            Byte ofs = ((csr_x - 5) / 3) + ((csr_y - 1) * 8);
            Byte digit = ((csr_x + 1) % 3) - 1;
            Word addr = mem_bank[0];
            if (csr_y > 10 && csr_y < 20) { ofs -= 80; addr = mem_bank[1]; }
            if (csr_y > 20) { ofs -= 160; addr = mem_bank[2]; }
            Byte data = Bus::Read(addr + ofs,true);
            if (digit == 0)		data = (data & 0x0f) | (ch << 4);
            if (digit == 1)		data = (data & 0xf0) | (ch << 0);
            Bus::Write(addr + ofs, data);
            if (csr_x < 28)		while (!CoordIsValid(++csr_x, csr_y));
            break;
        }
    }    
}

void Debug::_correctMouseCoords(int& mx, int& my)
{ 
    SDL_GetMouseState(&mx, &my);
    // printf("MX:%d MY:%d\n", mx, my);
    
    int ww, wh;
    SDL_GetWindowSize(Debug::GetSDLWindow(), &ww, &wh);

    float fw = ww;
    float fh = wh;
    int tw = DEBUG_WIDTH;
    int th = DEBUG_HEIGHT;
    SDL_Rect dest = { int(ww / 2 - (int)fw / 2), int(wh / 2 - (int)fh / 2), (int)fw, (int)fh };
    float w_aspect = (float)dest.w / tw;  
    float h_aspect = (float)dest.h / th;  
    mx = int((mx / w_aspect) - (dest.x / w_aspect));
    my = int((my / h_aspect) - (dest.y / h_aspect));
    mx /= 8;
    my /= 8;

    // printf("TW:%d  TH:%d   MX:%d   MY:%d\n",
    //    tw,
    //    th,
    //    mx, my);

    return;
}


void Debug::MouseStuff()
{
    if (!bIsMouseOver)  return;

    C6809* cpu = Bus::GetC6809();
    int mx, my;
    _correctMouseCoords(mx, my);
    Uint32 btns = SDL_GetRelativeMouseState(NULL, NULL);

    // mouse wheel
    if (mouse_wheel)
    {
        // scroll memory banks
        if (mx > 0 && mx < 29)
        {
            if (my > 0 && my < 10)	mem_bank[0] -= mouse_wheel * 8;
            if (my > 10 && my < 20)	mem_bank[1] -= mouse_wheel * 8;
            if (my > 20 && my < 30)	mem_bank[2] -= mouse_wheel * 8;
            bIsCursorVisible = false;
        }
        // Scroll the Code
        if (mx > 38 && mx < 64 && my > 5 && my < 30)
        {
            if (bMouseWheelActive == false)
            {
                mousewheel_offset = -25;
                bMouseWheelActive = true;
            }
            s_bSingleStep = true;	// scrollwheel enters into single step mode
            nRegisterBeingEdited.reg = Debug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
            mousewheel_offset -= mouse_wheel * 1;		// slow scroll
            if (SDL_GetModState() & KMOD_CTRL)	// is CTRL down?
                mousewheel_offset -= mouse_wheel * 3;	// faster scroll			
        }
        // scroll the break point display window (bottom right corner)
        if (mx >= 55 && my >= 33)
        {
            // printf("mouse_wheel: %d\n", mouse_wheel);
            mw_brk_offset -= mouse_wheel;
        }

        // reset the wheel
        mouse_wheel = 0;
    }

    // left mouse button clicked?
    static bool last_LMB = false;
    if ((btns & 1) && !last_LMB)
    {
        // left-clicked on breakpoint
        if (mx >= 55 && my >= 33)
        {
            int index = (my - 33) + mw_brk_offset;
            // build a vector of active breakpoints
            std::vector<Word> breakpoints;
            for (auto& bp : mapBreakpoints)
                if (bp.second)
                    breakpoints.push_back(bp.first);
            if ((unsigned)index < breakpoints.size())
                printf("LEFT CLICK: $%04X\n", breakpoints[index]);
            //mapBreakpoints[breakpoints[index]] = false;
        }

        // click to select
        if (btns & 1)
        {
            if (CoordIsValid(mx, my))
            {
                csr_x = mx;
                csr_y = my;
                bIsCursorVisible = true;
            }
            else
                bIsCursorVisible = false;
            //printf("MX:%d  MY:%d\n", mx, my);
        }
        // condition code clicked?
        if (my == 1)
        {
            if (mx == 48) cpu->setCC_E(!cpu->getCC_E());
            if (mx == 49) cpu->setCC_F(!cpu->getCC_F());
            if (mx == 50) cpu->setCC_H(!cpu->getCC_H());
            if (mx == 51) cpu->setCC_I(!cpu->getCC_I());
            if (mx == 52) cpu->setCC_N(!cpu->getCC_N());
            if (mx == 53) cpu->setCC_Z(!cpu->getCC_Z());
            if (mx == 54) cpu->setCC_V(!cpu->getCC_V());
            if (mx == 55) cpu->setCC_C(!cpu->getCC_C());
        }
        // Register Clicked?
        bool bFound = false;
        for (auto& a : register_info)
        {
            if (a.y_pos == my && mx >= a.x_min && mx <= a.x_max)
            {
                // begin editing a register
                nRegisterBeingEdited.reg = a.reg;
                nRegisterBeingEdited.value = a.value;
                nRegisterBeingEdited.y_pos = a.y_pos;
                nRegisterBeingEdited.x_min = a.x_min;
                nRegisterBeingEdited.x_max = a.x_max;
                csr_x = mx;
                csr_y = my;
                bFound = true;
            }
        }
        if (!bFound)
            nRegisterBeingEdited.reg = EDIT_NONE;
        // left-click on code line toggles breakpoint
        if (mx > 38 && mx < 64 && my > 5 && my < 30 && s_bSingleStep)
        {
            Word offset = sDisplayedAsm[my - 6];
            (mapBreakpoints[offset]) ?
                mapBreakpoints[offset] = false :
                mapBreakpoints[offset] = true;
        }
    }
    last_LMB = (btns & 1);
    // right mouse button clicked
    static bool last_RMB = false;
    if (btns & 4 && !last_RMB)
    {
        // right-clicked on breakpoint
        if (mx >= 55 && my >= 33)
        {
            int index = (my - 33) + mw_brk_offset;
            // build a vector of active breakpoints
            std::vector<Word> breakpoints;
            for (auto& bp : mapBreakpoints)
                if (bp.second)
                    breakpoints.push_back(bp.first);
            if ((unsigned)index < breakpoints.size())
            {
                //printf("RIGHT CLICK: $%04X\n", breakpoints[index]);
                mapBreakpoints[breakpoints[index]] = false;
            }
        }

        // on PC register
        if (my == 4 && mx > 42 && mx < 47)
        {
            s_bSingleStep = !s_bSingleStep;
            if (!s_bSingleStep)
                nRegisterBeingEdited.reg = Debug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
        }
        // right-click on code line toggles breakpoint and resumes execution
        if (mx > 38 && mx < 64 && my > 5 && my < 30 && s_bSingleStep)
        {
            Word offset = sDisplayedAsm[my - 6];
            (mapBreakpoints[offset]) ?
                mapBreakpoints[offset] = false :
                mapBreakpoints[offset] = true;
            if (mapBreakpoints[offset] == true)
                s_bSingleStep = false;
        }
    }
    last_RMB = (btns & 4);
}

bool Debug::CoordIsValid(int x, int y)
{
    if (y > 0 && y < 30 && y != 10 && y != 20)
    {
        // at an address
        if (x > 0 && x < 5)
        {
            csr_at = CSR_AT::CSR_AT_ADDRESS;
            return true;
        }
        // at a data entry
        if (x > 5 && x < 29 && (x + 1) % 3)
        {
            csr_at = CSR_AT::CSR_AT_DATA;
            return true;
        }
    }
    return false;
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










// ********************
// * Button Callbacks *
// ********************

void Debug::cbClearBreaks()
{
    if (mapBreakpoints.size() > 0)
        mapBreakpoints.clear();
}
void Debug::cbReset()
{
    C6809* cpu = Bus::GetC6809();
    cpu->reset();
    mousewheel_offset = 0;
    bMouseWheelActive = false;
    s_bSingleStep = true;
    s_bIsStepPaused = true;
}
void Debug::cbNMI()
{
    C6809* cpu = Bus::GetC6809();
    cpu->nmi();
    s_bIsStepPaused = false;
}
void Debug::cbIRQ()
{
    C6809* cpu = Bus::GetC6809();
    cpu->irq();
    s_bIsStepPaused = false;
}
void Debug::cbFIRQ()
{    
    C6809* cpu = Bus::GetC6809();
    cpu->firq();
    s_bIsStepPaused = false;
}
void Debug::cbRunStop()
{
    (s_bSingleStep) ? s_bSingleStep = false : s_bSingleStep = true;
    s_bIsStepPaused = s_bSingleStep;
    nRegisterBeingEdited.reg = Debug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
    bMouseWheelActive = false;
}
void Debug::cbHide()
{
    bMouseWheelActive = false;
    s_bSingleStep = false;
    s_bIsStepPaused = s_bSingleStep;
    nRegisterBeingEdited.reg = Debug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits

    s_bIsDebugActive = false;
    SDL_MinimizeWindow(Debug::GetSDLWindow());
}
void Debug::cbStepIn()  //F11
{
    s_bSingleStep = true;
    s_bIsStepPaused = false;
    nRegisterBeingEdited.reg = Debug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
    bMouseWheelActive = false;
}
void Debug::cbStepOver() //F10
{
    s_bSingleStep = true;
    s_bIsStepPaused = false;
    nRegisterBeingEdited.reg = Debug::EDIT_REGISTER::EDIT_NONE;	// cancel any register edits
    bMouseWheelActive = false;
}
void Debug::cbAddBrk()
{
    printf("Add Breakpoint\n");
    bEditingBreakpoint = true;
    nRegisterBeingEdited.reg = Debug::EDIT_REGISTER::EDIT_BREAK;
}
