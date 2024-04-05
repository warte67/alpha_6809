//////////
//
//  Bus.h  (thread-safe singleton)
//
/////////////

#include <chrono>
#include <sstream>
#include <fstream>
#include "Bus.hpp"
#include "Gfx.hpp"
#include "Debug.hpp"
#include "C6809.hpp"
#include "Mouse.hpp"

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
			printf(";  memory_map.asm\n\n");
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

    // standard video buffer
    dev->DisplayEnum("",0, "");
    std::stringstream ss;
    ss << "Video Buffer (" << (VID_BUFFER_SIZE/1024) << "K)";
    dev->DisplayEnum("",0x0400, ss.str());
    dev = new RAM("VIDEO_START");
    addr += Attach(dev, VID_BUFFER_SIZE);  
    dev->DisplayEnum("VIDEO_END",0x0400 + VID_BUFFER_SIZE - 1, "End of Default Video Space");  

	// user RAM
    Word uram_size = 0xB000 - addr;
    std::stringstream ss2;
    ss2 << "User Ram (" << uram_size/1024 << "K)";
    // printf("URAM: $%04x\n", uram_size);
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",addr, ss2.str());
    dev = new RAM("USER_RAM");
    addr += Attach(dev, uram_size);    

	// paged RAM
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",addr, "Paged Memory Bank One (8K)");
    dev = new RAM("PG_BANK_ONE");
    addr += Attach(dev, 8*1024);   

	// paged ROM
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",addr, "Paged Memory Bank Two (8K)");
    dev = new RAM("PG_BANK_TWO");
    addr += Attach(dev, 8*1024);          

	// KERNEL ROM
    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",addr, "KERNEL ROM (3.5K)");
    dev = new ROM("KERNEL_ROM");
    addr += Attach(dev, 3.5*1024);   // hardware registers should be included here

    ////////////////////////////////////////////////////////////
    // TODO:  
    //    Hardware Registers
    /////////

    dev->DisplayEnum("",0, "");
    dev->DisplayEnum("",0, "HARDWARE REGISTERS (0.5K)");

    // base graphics device
    s_gfx = new Gfx();  // non-enforced singleton
    addr += Attach(s_gfx);

    // attach the debug device
    s_debug = new Debug();
    addr += Attach(s_debug);

    // attach the mouse device
    s_mouse = new Mouse();
    addr += Attach(s_mouse);





    


    // RESERVED
    int reserved = 0xfff0 - addr;
    std::stringstream s;
    addr += reserved;
    dev = new ROM("RESERVED");
    addr += Attach(dev, reserved);
    s << reserved << " bytes in reserve";
    dev->DisplayEnum("",addr, s.str());

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

    // Load the Kernal ROM
    load_hex("kernel_f000.hex");

    // Call OnInit() before attaching the CPU Device
    OnInit();   // One time initialization

	// Install the CPU and start its thread
	s_c6809 = new C6809(this);
	s_cpuThread = std::thread(&C6809::ThreadProc);
	C6809::IsCpuEnabled(true);       
}

Bus::~Bus()
{
    // std::cout << "~" << Name() << "::Bus()\n";

    // shutdown the CPU thread
    s_cpuThread.join();

    // Remove the CPU device
    if (s_c6809)
	{
		delete s_c6809;
		s_c6809 = nullptr;
	}

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
                C6809::IsCpuEnabled(false);
                // shutdown the old environment
                OnDeactivate();
                // create a new environment
                OnActivate();


                // **************************************************
                // * FOR NOW WE JUST LEAVE THE CPU SITTING IDLE!!!! *
                // **************************************************
                C6809::IsCpuEnabled(true);



                // no longer dirty
                s_bIsDirty = false;            
            }
            OnUpdate(0.0f);
            OnEvent(nullptr);
            OnRender();      
            // only a present for GfxCore            
            Gfx::Present();
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
        switch (evnt.type) {
            // handle default events SDL_QUIT and ALT-X quits

            case SDL_QUIT:  
                // handling of close button 
                //          (this only works for single window applications. 
                //          Use SDL_WINDOWEVENT_CLOSE instead.)
                s_bIsRunning = false;
                break;

            case SDL_KEYDOWN:
            {
                // [ESCAPE]  KMOD_SHIFT
                SDL_Keymod mod = SDL_GetModState();
                if (mod & KMOD_SHIFT)
                {
                    if (evnt.key.keysym.sym == SDLK_ESCAPE)
                        s_bIsRunning = false;
                }                
                if (mod & KMOD_ALT)
                {
                    if (evnt.key.keysym.sym == SDLK_x)
                        s_bIsRunning = false;
                }
                break;                
            }
        }
        // OnEvent(evnt)
        for (auto &d : Bus::_memoryNodes)
            d->OnEvent(&evnt);		
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

        _sys_cpu_speed = (int)(1.0f / (s_avg_cpu_cycle_time / 1000000.0f));

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
	std::cout << "\n    ERROR: " << sErr << " -- " << SDL_GetError() << "\n\n";

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




// load_hex helpers
Byte Bus::_fread_hex_byte(std::ifstream& ifs)
{
	char str[3];
	long l;
	ifs.get(str[0]);
	ifs.get(str[1]);
	str[2] = '\0';	
	l = strtol(str, NULL, 16);
	return (Byte)(l & 0xff);	
}
Word Bus::_fread_hex_word(std::ifstream& ifs)
{
	Word ret;
	ret = _fread_hex_byte(ifs);
	ret <<= 8;
	ret |= _fread_hex_byte(ifs);
	return ret;				
}
void Bus::load_hex(const char* filename)
{
	// // lambda to convert integer to hex string
	// auto hex = [](uint32_t n, uint8_t digits)
	// {
	// 	std::string s(digits, '0');
	// 	for (int i = digits - 1; i >= 0; i--, n >>= 4)
	// 		s[i] = "0123456789ABCDEF"[n & 0xF];
	// 	return s;
	// };

	std::ifstream ifs(filename);
	if (!ifs.is_open())
	{
		//printf("UNABLE TO OPEN FILE '%s'\n", filename);	

        std::stringstream ss;
        ss << "Unable to open file: " << filename;
        Bus::Error(ss.str());        	

		return;
	}
	bool done = false;
	char c;	
	while (!done)
	{
		Byte n, t;
		Word addr;	
		Byte b;
		ifs.get(c);	// skip the leading ":"
		n = _fread_hex_byte(ifs);		// byte count for this line
		addr = _fread_hex_word(ifs);	// fetch the begin address		
		t = _fread_hex_byte(ifs);		// record type
		if (t == 0x00) 
		{
			while (n--) 
			{
				b = _fread_hex_byte(ifs);
				// std::cout << "0x" << hex(addr,4) << ":";
				// std::cout << "0x" << hex(b, 2) << std::endl;
				Bus::Write(addr, b, true);
				++addr;
			}
			// Read and discard checksum byte
			(void)_fread_hex_byte(ifs);	
			// skip the junk at the end of the line	
			if (ifs.peek() == '\r')	ifs.get(c);
			if (ifs.peek() == '\n')	ifs.get(c);
		}
		else if (t == 0x01) 
			done = true;
	}
	// close and return
	ifs.close();
	
	return;
}

