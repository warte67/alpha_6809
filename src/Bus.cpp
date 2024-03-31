//////////
//
//  Bus.h  (thread-safe singleton)
//
/////////////

#include <chrono>
#include <sstream>
#include "Bus.hpp"
#include "GfxVRam.hpp"
#include "Gfx.hpp"


Bus::Bus()
{
    // std::cout << Name() << "::Bus()\n";

    _deviceName = "Bus";

    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::stringstream ss;
        ss << "SDL_Init() failed!\n" << SDL_GetError() << std::endl;
        Bus::Error(ss.str());
        s_bIsRunning = false;
        return;     // no need to continue if SDL_Init fails
    }    
    s_b_SDL_WasInit = true;

    if (COMPILE_MEMORY_MAP)
    {
        if (MEMORY_MAP_OUTPUT_CPP)		// C++ version
        {
			printf("\n\n");
			printf("// memory_map.h\n");
			printf("#ifndef __MEMORY_MAP_H__\n");
			printf("#define __MEMORY_MAP_H__\n");
            printf("\n");
            printf("enum MEMMAP\n");
            printf("{\n");
            printf("    //  **********************************************\n");
            printf("    //  * Allocated 64k Memory Mapped System Symbols *\n");
            printf("    //  **********************************************\n");         
        }
		else							// 6809 ASM version
		{
			printf("\n\n");
			printf("// memory_map.asm\n");
            printf(";  **********************************************\n");
            printf(";  * Allocated 64k Memory Mapped System Symbols *\n");
            printf(";  **********************************************\n");
        }
    }



    // initialize all of the devices that are to be attached to the bus

	// SOFT VECTORS
	IDevice* dev = nullptr;
	dev = new RAM("SOFT_VECTORS");
	int addr = Attach(dev, 16);
    dev->DisplayEnum("", 0, "");
    dev->DisplayEnum("", 0,"Software Interrupt Vectors:");
    dev->DisplayEnum("SOFT_EXEC", 0x0000, "Exec Software Interrupt Vector");
    dev->DisplayEnum("SOFT_SWI3", 0x0002, "SWI3 Software Interrupt Vector");
    dev->DisplayEnum("SOFT_SWI2", 0x0004, "SWI2 Software Interrupt Vector");
    dev->DisplayEnum("SOFT_FIRQ", 0x0006, "FIRQ Software Interrupt Vector");
    dev->DisplayEnum("SOFT_IRQ",  0x0008, "IRQ Software Interrupt Vector");
    dev->DisplayEnum("SOFT_SWI",  0x000A, "SWI / SYS Software Interrupt Vector");
    dev->DisplayEnum("SOFT_NMI",  0x000C, "NMI Software Interrupt Vector");
    dev->DisplayEnum("SOFT_RESET",0x000E, "RESET Software Interrupt Vector");	
	dev->DisplayEnum("", 0, "");

	// zero page 256 bytes	
    dev->DisplayEnum("", 0, "System Memory:");
    dev = new RAM("ZERO_PAGE");
    addr += Attach(dev, 256 - 16);	

    // user stack 256 bytes
    dev = new RAM("USER_STACK");
    addr += Attach(dev, 256);
    dev->DisplayEnum("USTACK_TOP", 0x0200, "Top of the user stack space");

	// system stack 512 (768) bytes
    dev = new RAM("SYSTEM_STACK");
    addr += Attach(dev, 512);  
    dev->DisplayEnum("SSTACK_TOP",0x0400, "Top of the system stack space");  

    // standard video buffer (11k)
    dev->DisplayEnum("",0, "");
    std::stringstream ss;
    ss << "Video Buffer (" << (VID_BUFFER_SIZE/1024) << "K)";
    dev->DisplayEnum("",0x0400, ss.str());
    s_gfx_vram = new GfxVRam();
    addr += Attach(s_gfx_vram);    
    
	// user RAM
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",0x3000, "User RAM (32K)");
    dev = new RAM("USER_RAM");
    addr += Attach(dev, 32*1024);    

	// paged RAM
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",0xB000, "Paged Memory Bank One (8K)");
    dev = new RAM("PG_BANK_ONE");
    addr += Attach(dev, 8*1024);   

	// paged ROM
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",0xD000, "Paged Memory Bank Two (8K)");
    dev = new RAM("PG_BANK_TWO");
    addr += Attach(dev, 8*1024);          

	// KERNEL ROM
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",0xF000, "KERNEL ROM (3.5K)");
    dev = new ROM("KERNEL_ROM");
    addr += Attach(dev, 3.5*1024);   // hardware registers should be included here

    ////////////////////////////////////////////////////////////
    // TODO:  
    //    Hardware Registers
    /////////

    // ...
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",0xF000, "HARDWARE REGISTERS (0.5K)");
    s_gfx = new Gfx();  // non-enforced singleton
    addr += Attach(s_gfx);

    // RESERVED
    int reserved = 0xfff0 - addr;
    std::stringstream s;
    s << reserved << " bytes in reserve";
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",addr, s.str());
    addr += reserved;
    dev = new ROM("RESERVED");
    addr += Attach(dev, reserved);





	// ROM VECTORS
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",0,"Hardware Interrupt Vectors:");
    dev = new ROM("ROM_VECTS");
    addr += Attach(dev, 16);
    dev->DisplayEnum("HARD_EXEC", 0xfff0, "EXEC Hardware Interrupt Vector");
    dev->DisplayEnum("HARD_SWI3", 0xfff2, "SWI3 Hardware Interrupt Vector");
    dev->DisplayEnum("HARD_SWI2", 0xfff4, "SWI2 Hardware Interrupt Vector");
    dev->DisplayEnum("HARD_FIRQ", 0xfff6, "FIRQ Hardware Interrupt Vector");
    dev->DisplayEnum("HARD_IRQ",  0xfff8, "IRQ Hardware Interrupt Vector");
    dev->DisplayEnum("HARD_SWI",  0xfffA, "SWI / SYS Hardware Interrupt Vector");
    dev->DisplayEnum("HARD_NMI",  0xfffC, "NMI Hardware Interrupt Vector");
    dev->DisplayEnum("HARD_RESET",0xfffE, "RESET Hardware Interrupt Vector");

    // close the memory map definitions
    if (COMPILE_MEMORY_MAP)
    {
        if (MEMORY_MAP_OUTPUT_CPP)
		{
            printf("};  // END: enum MEMMAP\n\n");
			printf("\n");
			printf("#endif // __MEMORY_MAP_H__\n");
			printf("\n\n");
		}
        else
        {
            printf("; END of definitions\n\n");
			printf("\n\n");
        }

        printf("\n\nThe flag COMPILE_MEMORY_MAP is set as true, therefore this app\n");
        printf("cannot continue past this point. Set the definition of COMPILE_MEMORY_MAP\n");
        printf("to false in the file types.h and recompile to run this application.\n\n");
        Error("Copy the above enumerated definitions above to memory_map.h");
        //exit(0);
    }


    // Call OnInit() before attaching the CPU Device
    OnInit();   // One time initialization

    // set up the CPU Device thread
    // ...
}

Bus::~Bus()
{
    // std::cout << "~" << Name() << "::Bus()\n";

    // shutdown the CPU thread
    // ...

    // Remove the CPU device
    // ...

    // delete all of the attached devices
    for (auto& d : Bus::_memoryNodes)
        delete d;

    // close SDL
    if (s_b_SDL_WasInit)
    {
        SDL_Quit();
    }
}


void Bus::Run()
{    
    // std::cout << Name() << "::Run()\n";

    // Bus::Error("Does the error thing work?");

    if (s_bIsRunning)
    {
        while (s_bIsRunning)
        {
            if (s_bIsDirty)
            {
                // shutdown the old environment
                OnDeactivate();
                // create a new environment
                OnActivate();
                // no longer dirty
                s_bIsDirty = false;            
            }
            OnUpdate(0.0f);
            OnEvent(nullptr);
            OnRender();      
            // only a present for Gfx
            // m_gfx->OnPresent();  // special case for the GFX Device
        }
        // shutdown the environment
        OnDeactivate();    

        // close down all of the attached devices
        OnQuit();   // One time destructor
    }
}




Word Bus::OnAttach(Word nextAddr)
{
    // this should never actually be called from the Bus
    return 0;

    // std::cout << Name() << "::OnAttach()\n";

    return 0;
}

void Bus::OnInit()
{
    // std::cout << Name() << "::OnInit()\n";
	for (auto &d : Bus::_memoryNodes)
		d->OnInit();    
}

void Bus::OnQuit()
{
    // std::cout << Name() << "::OnQuit()\n";
	for (auto &d : Bus::_memoryNodes)
		d->OnQuit();    
}

void Bus::OnActivate()
{
    // std::cout << Name() << "::OnActivate()\n";
	for (auto &d : Bus::_memoryNodes)
		d->OnActivate();    
}
void Bus::OnDeactivate()
{
    // std::cout << Name() << "::OnDeactivate()\n";
    for (auto &d : Bus::_memoryNodes)
		d->OnDeactivate();
}

void Bus::OnEvent(SDL_Event* null_event)
{ 
    // std::cout << Name() << "::OnEvent()\n";

    SDL_Event evnt;
    while (SDL_PollEvent(&evnt))
    {
        // OnEvent(evnt)
		for (auto &d : Bus::_memoryNodes)
			d->OnEvent(&evnt);		

        switch (evnt.type) {
            // handle default events SDL_QUIT and ALT-X quits
            case SDL_QUIT:
                // handling of close button
                s_bIsRunning = false;
                break;
            
            case SDL_KEYDOWN:
                // [ALT-X]
                if (evnt.key.keysym.sym == SDLK_x)
                {
                    if (SDL_GetModState() & KMOD_ALT)
                        s_bIsRunning = false;
                }
                // [ESCAPE]
                if (evnt.key.keysym.sym == SDLK_ESCAPE)
                    s_bIsRunning = false;
                break;
        }
    }        
}

Byte Bus::clock_div(Byte& cl_div, int bit)
{
    if (bit > 7)    bit = 7;
    double count[] =
    { // pulse width:   // frequency:
        8.33333,        // 60 hz
        16.66667,       // 30 hz
        33.33333,       // 15 hz
        66.66667,       // 7.5 hz
        133.33333,      // 3.75 hz
        266.66667,      // 1.875 hz
        533.33333,      // 0.9375 hz
        1066.66667,     // 0.46875
    };

    using clock = std::chrono::system_clock;
    using sec = std::chrono::duration<double, std::milli>;
    static auto before0 = clock::now();
    static auto before1 = clock::now();
    static auto before2 = clock::now();
    static auto before3 = clock::now();
    static auto before4 = clock::now();
    static auto before5 = clock::now();
    static auto before6 = clock::now();
    static auto before7 = clock::now();
    static auto before = clock::now();
    switch (bit)
    {
    case 0: before = before0; break;
    case 1: before = before1; break;
    case 2: before = before2; break;
    case 3: before = before3; break;
    case 4: before = before4; break;
    case 5: before = before5; break;
    case 6: before = before6; break;
    case 7: before = before7; break;
    }
    const sec duration = clock::now() - before;
    if (duration.count() > count[bit])
    {
        before = clock::now();
        switch (bit)
        {
        case 0: before0 = clock::now();   _clock_timer++;  break;
        case 1: before1 = clock::now();  break;
        case 2: before2 = clock::now();  break;
        case 3: before3 = clock::now();  break;
        case 4: before4 = clock::now();  break;
        case 5: before5 = clock::now();  break;
        case 6: before6 = clock::now();  break;
        case 7: before7 = clock::now();  break;
        }
        cl_div = (cl_div & (0x01 << bit)) ? cl_div & ~(0x01 << bit) : cl_div | (0x01 << bit);
    }
    return cl_div;
}


void Bus::clockDivider()
{
    // static Byte cl_div = 0;
    for (int bit = 0; bit < 8; bit++)
        clock_div(_clock_div, bit);
}

void Bus::OnUpdate(float fNullTime)
{
    // std::cout << Name() << "::OnUpdate()\n";

    // update the clock divider
    clockDivider();

	// Handle Timing
    static std::chrono::time_point<std::chrono::system_clock> tp1 = std::chrono::system_clock::now();
    static std::chrono::time_point<std::chrono::system_clock> tp2 = std::chrono::system_clock::now();

    tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = tp2 - tp1;
    tp1 = tp2;
    // Our time per frame coefficient
    float fElapsedTime = elapsedTime.count();
    // static float fLastElapsed = fElapsedTime;
    // static float fFrameTimer = fElapsedTime;

    // count frames per second
    // static int fps = 0;
    static int frame_count = 0;
    static float frame_acc = fElapsedTime;
    frame_count++;
    frame_acc += fElapsedTime;    

    if (frame_acc > 0.25f + fElapsedTime)
    {
        frame_acc -= 0.25f;
		_fps = frame_count * 4;
		frame_count = 0;

		// clean this up (get rid of the requirement for th GFX friend class Bus bullshit)
		std::string sTitle = "Retro 6809";
		sTitle += "  FPS: ";
		sTitle += std::to_string(_fps);

        _sys_cpu_speed = (int)(1.0f / (_avg_cpu_cycle_time / 1000000.0f));

        sTitle += "   CPU_SPEED: " + std::to_string(_sys_cpu_speed) + " khz.";
		// if (m_gfx)
		// 	if (m_gfx->_window)
		// 		 SDL_SetWindowTitle(m_gfx->_window, sTitle.c_str());
    }    

	// update the devices
	for (auto &d : Bus::_memoryNodes)
		d->OnUpdate(fElapsedTime);	    
}

void Bus::OnRender()
{
    // std::cout << Name() << "::OnRender()\n";
	for (auto &d : Bus::_memoryNodes)
		d->OnRender();    
}

Word Bus::Attach(IDevice* dev, Word size) 
{    
    // std::cout << Name() << "::Attach()\n";
    if (dev != nullptr)
    {
        if (size == 0)
            size = dev->OnAttach((Word)_lastAddress);     
        else
            dev->DisplayEnum(dev->Name(), _lastAddress, "");
        dev->Base(_lastAddress);
        dev->Size(size);
        _lastAddress += size;               
        Bus::_memoryNodes.push_back(dev);
    }
    if (size > 65536)
        Bus::Error("Memory allocation beyond 64k boundary!");
    return size;
}

void Bus::Error(const std::string& sErr)
{
	std::cout << "\n    ERROR: " << sErr << SDL_GetError() << "\n\n";

    // if SDL has been initialized, use a message box too
    if (s_b_SDL_WasInit)
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR:", sErr.c_str(), nullptr);

    s_bIsRunning = false;
}



Byte Bus::read(Word offset, bool debug) 
{
	return Bus::Read(offset, debug);
}

void Bus::write(Word offset, Byte data, bool debug) 
{
	Bus::Write(offset, data, debug);
}

Word Bus::read_word(Word offset, bool debug) 
{
	return Bus::Read_Word(offset, debug);
}

void Bus::write_word(Word offset, Word data, bool debug)
{
	Bus::Write_Word(offset, data, debug);
}




Byte Bus::Read(Word offset, bool debug)
{
    for (auto& a : Bus::_memoryNodes)
    {
        if (offset - a->Base() < a->Size())
        {
            if (debug)
                if (offset - a->Base() < a->Size())
                    return a->_memory((Word)(offset - a->Base()));

            return a->read(offset, debug);
        }
    }
    return 0xCC;
}

void Bus::Write(Word offset, Byte data, bool debug)
{
    for (auto& a : Bus::_memoryNodes)
    {
        if (offset - a->Base() < a->Size())
        {
            if (debug)
            {
                if (offset - a->Base() < a->Size())
                    a->_memory((Word)(offset - a->Base()), data);
                return;
            }
            a->write(offset, data, debug);
            return;
        }
    }
}
Word Bus::Read_Word(Word offset, bool debug)
{
    Word ret = (Bus::Read(offset) << 8) | Bus::Read(offset + 1);
    return ret;
}
void Bus::Write_Word(Word offset, Word data, bool debug)
{
    Byte msb = (data >> 8) & 0xFF;
    Byte lsb = data & 0xff;
    Bus::Write(offset, msb);
    Bus::Write(offset + 1, lsb);
}


void Bus::def_display()
{
}
