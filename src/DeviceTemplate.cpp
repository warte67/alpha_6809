// *************************************************
// *
// * DeviceTemplate.cpp
// *
// ***********************************

#include "DeviceTemplate.hpp"

Byte DeviceTemplate::read(Word offset, bool debug) 
{
    Byte data = IDevice::read(offset);
    printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    // ...

    IDevice::write(offset,data);   // update any internal changes too
    return data;
}
void DeviceTemplate::write(Word offset, Byte data, bool debug) 
{
    printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    IDevice::write(offset,data);   // update any internal changes too
}





Word DeviceTemplate::OnAttach(Word nextAddr) 
{
    printf("%s::OnAttach()\n", Name().c_str());    
    return 0;
}
void DeviceTemplate::OnInit() 
{
    printf("%s::OnInit()\n", Name().c_str());    
}
void DeviceTemplate::OnQuit() 
{
    printf("%s::OnQuit()\n", Name().c_str());    
}
void DeviceTemplate::OnActivate() 
{
    printf("%s::OnActivate()\n", Name().c_str());    
}
void DeviceTemplate::OnDeactivate() 
{
    printf("%s::OnDeactivate()\n", Name().c_str());    
}
void DeviceTemplate::OnEvent(SDL_Event* evnt) 
{
    printf("%s::OnEvent()\n", Name().c_str());    
}
void DeviceTemplate::OnUpdate(float fElapsedTime) 
{
    printf("%s::OnUpdate()\n", Name().c_str());    
}
void DeviceTemplate::OnRender() 
{
    printf("%s::OnRender()\n", Name().c_str());    
}

