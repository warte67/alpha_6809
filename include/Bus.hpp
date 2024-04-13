//////////
//
//  Bus.h  (thread-safe singleton)
//
/////////////

#pragma once

#include <thread>
#include "IDevice.hpp"

class GfxCore;
class Gfx;
class Debug;
class C6809;
class Mouse;
class Keyboard;
class Gamepad;
class FileIO;
class Math;
class MemBank;

class Bus : public IDevice
{
    friend class C6809;     // This is cheating? Maybe not, but it does make me feel a bit dirty.
    friend class Debug;     // Again, this feels so wrong!

    private:
        Bus();

        // private statics
        inline static bool s_bIsRunning = true; 
        inline static bool s_bIsDirty = true;
        inline static bool s_b_SDL_WasInit = false;

        inline static int _fps = 0;

		inline static float s_avg_cpu_cycle_time = 0;
		inline static Byte _clock_div = 0;				// SYS_CLOCK_DIV (Byte) 60 hz Clock Divider  (Read Only) 
		inline static Word _clock_timer = 0;			// SYS_TIMER	(R/W Word) increments at 0.46875 hz
		inline static Word _sys_cpu_speed = 0;			// SYS_SPEED	(Read Byte) register

        inline static std::thread s_cpuThread;

        inline static GfxCore* s_gfx_core = nullptr;
        inline static Gfx* s_gfx = nullptr;
        inline static Debug* s_debug = nullptr;
        inline static C6809* s_c6809 = nullptr;
        inline static Mouse* s_mouse = nullptr;
        inline static Keyboard* s_keyboard = nullptr;
        inline static Gamepad* s_gamepad = nullptr;
        inline static FileIO* s_fileio = nullptr;
        inline static Math* s_math = nullptr;
        inline static MemBank* s_membank = nullptr;

    public:
		~Bus();									// destructor
		Bus(const Bus&) = delete;				// copy constructor
		Bus(Bus&&) = delete;					// move constructor
			// allowing the copy and move assignment operators for now
			//Bus& operator=(const Bus&) = delete;	// copy assignment operator
			//Bus& operator=(Bus&&) = delete;		// move assignment operator
		static Bus& Inst() 						
		{ 
			static Bus inst; 
			return inst; 
		}        
        void Run();
        Word Attach(IDevice* dev, Word size = 0);   // attach a user defined memory node
        static void Error(const std::string& sErr);

        // pure virtuals
		Word OnAttach(Word nextAddr) override;
		void OnInit() override;
		void OnQuit() override;
		void OnActivate() override;
		void OnDeactivate() override;
		void OnEvent(SDL_Event* null_event) override;
		void OnUpdate(float fNullTime) override;
		void OnRender() override;

        Byte read(Word offset, bool debug = false) override;
        void write(Word offset, Byte data, bool debug = false) override;
        Word read_word(Word offset, bool debug = false) override;
        void write_word(Word offset, Word data, bool debug = false)  override;

		static Byte Read(Word offset, bool debug = false);
		static void Write(Word offset, Byte data, bool debug = false);
		static Word Read_Word(Word offset, bool debug = false);
		static void Write_Word(Word offset, Word data, bool debug = false);       

		static DWord Read_DWord(Word offset, bool debug = false);
        static void Write_DWord(Word offset, DWord data, bool debug = false);

        // inline static GfxCore* gfx_core() { return s_gfx_core; }    // to non-enforced singleton
        // inline static Gfx* gfx() { return s_gfx; }                  // to non-enforced singleton
        inline static bool IsDirty() { return s_bIsDirty; }
        inline static void IsDirty(bool dirty) { s_bIsDirty = dirty; }
        inline static bool IsRunning() { return s_bIsRunning; }
        inline static void IsRunning(bool _r) { s_bIsRunning = _r; }

        inline static Gfx* GetGfx() { return s_gfx; }
        inline static Debug* GetDebug() { return s_debug; }
        inline static C6809* GetC6809() { return s_c6809; }
        inline static Mouse* GetMouse() { return s_mouse; }
        inline static Keyboard* GetKeyboard() { return s_keyboard; }
        inline static Gamepad* GetGamepad() { return s_gamepad; }
        inline static FileIO* GetFileIO() { return s_fileio; }
        inline static Math* GetMath() { return s_math; }
        inline static MemBank* GetMemBank() { return s_membank; }

        void load_hex(const char* filename);

    private:
        int _lastAddress = 0;
        inline static std::vector<IDevice*> _memoryNodes;		

        // helpers
        Byte clock_div(Byte& cl_div, int bit);
        void clockDivider();

        struct mem_def_node
        {
            std::string label;
            int offset;
            std::string comment;
        };
        inline static std::vector<mem_def_node> vec_mem_def;
        inline static void def_push(std::string label, int offset, std::string comment) 
        {    
            // offset == -1 (none)            
            vec_mem_def.push_back({label, offset, comment});
        }	

        Byte _fread_hex_byte(std::ifstream& ifs);
        Word _fread_hex_word(std::ifstream& ifs);

};

