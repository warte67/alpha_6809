// *************************************************
// *
// * Debug.cpp
// *
// ***********************************

#include "Debug.hpp"

Byte Debug::read(Word offset, bool debug) 
{
    Byte data = IDevice::read(offset);
    printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    // ...

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}
void Debug::write(Word offset, Byte data, bool debug) 
{
    printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    IDevice::write(offset,data);   // update any internal changes too
}





Word Debug::OnAttach(Word nextAddr) 
{
    printf("%s::OnAttach()\n", Name().c_str());    
    return 0;
}
void Debug::OnInit() 
{
    printf("%s::OnInit()\n", Name().c_str());    
}
void Debug::OnQuit() 
{
    printf("%s::OnQuit()\n", Name().c_str());    
}
void Debug::OnActivate() 
{
    printf("%s::OnActivate()\n", Name().c_str());    
}
void Debug::OnDeactivate() 
{
    printf("%s::OnDeactivate()\n", Name().c_str());    
}
void Debug::OnEvent(SDL_Event* evnt) 
{
    printf("%s::OnEvent()\n", Name().c_str());    
}
void Debug::OnUpdate(float fElapsedTime) 
{
    printf("%s::OnUpdate()\n", Name().c_str());    
}
void Debug::OnRender() 
{
    printf("%s::OnRender()\n", Name().c_str());    
}

