//////////
//
//  Bus.h  (thread-safe singleton)
//
/////////////

#pragma once

#include "IDevice.hpp"

class GfxVRam;
class Gfx;

class Bus : public IDevice
{
    private:
        Bus();

        // private statics
        inline static bool s_bIsRunning = true; 
        inline static bool s_bIsDirty = true;
        inline static bool s_b_SDL_WasInit = false;

        inline static int _fps = 0;
		inline static Byte _clock_div = 0;				// SYS_CLOCK_DIV (Byte) 60 hz Clock Divider  (Read Only) 
		inline static Word _clock_timer = 0;			// SYS_TIMER	(R/W Word) increments at 0.46875 hz
        inline static Word _sys_cpu_speed = 0;			// SYS_SPEED	(Read Byte) register
        float _avg_cpu_cycle_time = 0.0f;

        inline static GfxVRam* s_gfx_vram = nullptr;
        inline static Gfx* s_gfx = nullptr;

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

        inline static GfxVRam* gfx_vram() { return s_gfx_vram; }    // to non-enforced singleton
        inline static Gfx* gfx() { return s_gfx; }                  // to non-enforced singleton

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
        static void def_display();		
};

