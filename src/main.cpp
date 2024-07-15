//
// Using CMAKE to build the Makefile:
//      to build the Makefile, run CMAKE from the build folder:
//          cmake .
//      to build the executable, run MAKE from the build folder:
//          make
//      to run the executable, run MAIN from the build folder:
//          ./main
//
// VSCode with the CMAKE plugins -- 
//      CMAKE Icon: 
//            Configure->Debug or Release
//            F5 to build Makefiles
//            CTRL-F5 to make and run (debug)
//
//
//
#include <SDL2/SDL.h>
#include <iostream>
#include "Bus.hpp"
    
int main(int argc, char *argv[])
{
    Bus& bus = Bus::Inst();
    bus.Run();

    #ifdef DEBUG
        std::cout << "DEBUG flag was set at compile time.\n";
    #endif

    return (int)0;
}