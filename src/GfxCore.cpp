// *************************************************
// *
// * GfxCore.cpp
// *
// ***********************************

#include <sstream>
#include "GfxCore.hpp"
#include "Bus.hpp"


Byte GfxCore::read(Word offset, bool debug)
{
    Byte data = IDevice::read(offset);
    // printf("%s::read($%04X) = $%02X\n", Name().c_str(), offset,  data);

    return data;
}

void GfxCore::write(Word offset, Byte data, bool debug)
{
    // printf("%s::write($%04X, $%02X)\n", Name().c_str(), offset, data);    

    IDevice::write(offset,data);
    // printf("Gfx::read($%04X) = $%02X\n", offset,  IDevice::read(offset));
}

Word GfxCore::OnAttach(Word nextAddr)
{
    // printf("%s::OnAttach()\n", Name().c_str());

    Word old_addr = nextAddr;
    DisplayEnum("VIDEO_START", nextAddr, "Start of Video Buffer Memory");
    nextAddr += VID_BUFFER_SIZE;
    DisplayEnum("VIDEO_END", nextAddr-1, "End of Video Buffer Memory");
    return nextAddr - old_addr;
}

void GfxCore::OnInit()
{
    // printf("%s::OnInit()\n", Name().c_str());

    _init_gmodes();
}

void GfxCore::OnQuit()
{
    // printf("%s::OnQuit()\n", Name().c_str());
}

void GfxCore::OnActivate()
{
    // printf("%s::OnActivate()\n", Name().c_str());

    if (!sdl_window)
    {
        sdl_window = SDL_CreateWindow("alpha_6809",
                              SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, 
                              512, 288,
                              SDL_WINDOW_RESIZABLE);
        if (!sdl_window)
        {
            std::stringstream ss;
            ss << "Unable to create the SDL window: " << SDL_GetError();
            Bus::Error(ss.str());
        }            
    }
}

void GfxCore::OnDeactivate()
{
    // printf("%s::OnDeactivate()\n", Name().c_str());

    // destroy the SDL Window
    if (sdl_window)
    {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }
}

void GfxCore::OnEvent(SDL_Event* evnt)
{
    // printf("%sw::OnEvent()\n", Name().c_str());
}

void GfxCore::OnUpdate(float fElapsedTime)
{
    // printf("%s::OnUpdate()\n", Name().c_str());
}

void GfxCore::OnRender()
{
    // printf("%s::OnRender()\n", Name().c_str());
}


// helpers

void GfxCore::_init_gmodes()
{
    /******
    // Timings
    vec_timings.push_back( { 1600,  900 } );             //  0 = 1600 x  900
    vec_timings.push_back( { 1440,  900 } );             //  1 = 1440 x  900
    vec_timings.push_back( { 1680, 1050 } );             //  2 = 1680 x 1050
    vec_timings.push_back( { 1280,  800 } );             //  3 = 1280 x  800
    vec_timings.push_back( { 1280,  720 } );             //  4 = 1280 x  720
    vec_timings.push_back( { 1024,  768 } );             //  5 = 1024 x  768
    vec_timings.push_back( {  800,  600 } );             //  6 =  800 x  600
    vec_timings.push_back( {  640,  480 } );             //  7 =  640 x  480

    // timing index = (vec_gmode index) % 32

    // 1600 x 900
    vec_gmodes.push_back( { 0, 800, 900, 2, 1 } );         //   0
    vec_gmodes.push_back( { 0, 800, 450, 2, 2 } );         //   1
    vec_gmodes.push_back( { 0, 800, 300, 2, 3 } );         //   2
    vec_gmodes.push_back( { 0, 800, 225, 2, 4 } );         //   3
    vec_gmodes.push_back( { 0, 800, 180, 2, 5 } );         //   4
    vec_gmodes.push_back( { 0, 800, 150, 2, 6 } );         //   5
    vec_gmodes.push_back( { 0, 800, 128, 2, 7 } );         //   6
    vec_gmodes.push_back( { 0, 800, 112, 2, 8 } );         //   7
    vec_gmodes.push_back( { 0, 400, 900, 4, 1 } );         //   8
    vec_gmodes.push_back( { 0, 400, 450, 4, 2 } );         //   9
    vec_gmodes.push_back( { 0, 400, 300, 4, 3 } );         //  10
    vec_gmodes.push_back( { 0, 400, 225, 4, 4 } );         //  11
    vec_gmodes.push_back( { 0, 400, 180, 4, 5 } );         //  12
    vec_gmodes.push_back( { 0, 400, 150, 4, 6 } );         //  13
    vec_gmodes.push_back( { 0, 400, 128, 4, 7 } );         //  14
    vec_gmodes.push_back( { 0, 400, 112, 4, 8 } );         //  15
    vec_gmodes.push_back( { 0, 320, 900, 5, 1 } );         //  16
    vec_gmodes.push_back( { 0, 320, 450, 5, 2 } );         //  17
    vec_gmodes.push_back( { 0, 320, 300, 5, 3 } );         //  18
    vec_gmodes.push_back( { 0, 320, 225, 5, 4 } );         //  19
    vec_gmodes.push_back( { 0, 320, 180, 5, 5 } );         //  20
    vec_gmodes.push_back( { 0, 320, 150, 5, 6 } );         //  21
    vec_gmodes.push_back( { 0, 320, 128, 5, 7 } );         //  22
    vec_gmodes.push_back( { 0, 320, 112, 5, 8 } );         //  23
    vec_gmodes.push_back( { 0, 200, 900, 8, 1 } );         //  24
    vec_gmodes.push_back( { 0, 200, 450, 8, 2 } );         //  25
    vec_gmodes.push_back( { 0, 200, 300, 8, 3 } );         //  26
    vec_gmodes.push_back( { 0, 200, 225, 8, 4 } );         //  27
    vec_gmodes.push_back( { 0, 200, 180, 8, 5 } );         //  28
    vec_gmodes.push_back( { 0, 200, 150, 8, 6 } );         //  29
    vec_gmodes.push_back( { 0, 200, 128, 8, 7 } );         //  30
    vec_gmodes.push_back( { 0, 200, 112, 8, 8 } );         //  31

    // 1440 x 900
    vec_gmodes.push_back( { 1, 720, 900, 2, 1 } );         //  32
    vec_gmodes.push_back( { 1, 720, 450, 2, 2 } );         //  33
    vec_gmodes.push_back( { 1, 720, 300, 2, 3 } );         //  34
    vec_gmodes.push_back( { 1, 720, 225, 2, 4 } );         //  35
    vec_gmodes.push_back( { 1, 720, 180, 2, 5 } );         //  36
    vec_gmodes.push_back( { 1, 720, 150, 2, 6 } );         //  37
    vec_gmodes.push_back( { 1, 720, 128, 2, 7 } );         //  38
    vec_gmodes.push_back( { 1, 720, 112, 2, 8 } );         //  39
    vec_gmodes.push_back( { 1, 480, 900, 3, 1 } );         //  40
    vec_gmodes.push_back( { 1, 480, 450, 3, 2 } );         //  41
    vec_gmodes.push_back( { 1, 480, 300, 3, 3 } );         //  42
    vec_gmodes.push_back( { 1, 480, 225, 3, 4 } );         //  43
    vec_gmodes.push_back( { 1, 480, 180, 3, 5 } );         //  44
    vec_gmodes.push_back( { 1, 480, 150, 3, 6 } );         //  45
    vec_gmodes.push_back( { 1, 480, 128, 3, 7 } );         //  46
    vec_gmodes.push_back( { 1, 480, 112, 3, 8 } );         //  47
    vec_gmodes.push_back( { 1, 360, 900, 4, 1 } );         //  48
    vec_gmodes.push_back( { 1, 360, 450, 4, 2 } );         //  49
    vec_gmodes.push_back( { 1, 360, 300, 4, 3 } );         //  50
    vec_gmodes.push_back( { 1, 360, 225, 4, 4 } );         //  51
    vec_gmodes.push_back( { 1, 360, 180, 4, 5 } );         //  52
    vec_gmodes.push_back( { 1, 360, 150, 4, 6 } );         //  53
    vec_gmodes.push_back( { 1, 360, 128, 4, 7 } );         //  54
    vec_gmodes.push_back( { 1, 360, 112, 4, 8 } );         //  55
    vec_gmodes.push_back( { 1, 288, 900, 5, 1 } );         //  56
    vec_gmodes.push_back( { 1, 288, 450, 5, 2 } );         //  57
    vec_gmodes.push_back( { 1, 288, 300, 5, 3 } );         //  58
    vec_gmodes.push_back( { 1, 288, 225, 5, 4 } );         //  59
    vec_gmodes.push_back( { 1, 288, 180, 5, 5 } );         //  60
    vec_gmodes.push_back( { 1, 288, 150, 5, 6 } );         //  61
    vec_gmodes.push_back( { 1, 288, 128, 5, 7 } );         //  62
    vec_gmodes.push_back( { 1, 288, 112, 5, 8 } );         //  63

    // 1680 x 1050
    vec_gmodes.push_back( { 2, 840, 525, 2, 2 } );         //  64
    vec_gmodes.push_back( { 2, 840, 350, 2, 3 } );         //  65
    vec_gmodes.push_back( { 2, 840, 210, 2, 5 } );         //  66
    vec_gmodes.push_back( { 2, 840, 175, 2, 6 } );         //  67
    vec_gmodes.push_back( { 2, 840, 150, 2, 7 } );         //  68
    vec_gmodes.push_back( { 2, 840, 105, 2,10 } );         //  69
    vec_gmodes.push_back( { 2, 840,  75, 2,14 } );         //  70
    vec_gmodes.push_back( { 2, 840,  70, 2,15 } );         //  71
    vec_gmodes.push_back( { 2, 420, 525, 4, 2 } );         //  72
    vec_gmodes.push_back( { 2, 420, 350, 4, 3 } );         //  73
    vec_gmodes.push_back( { 2, 420, 210, 4, 5 } );         //  74
    vec_gmodes.push_back( { 2, 420, 175, 4, 6 } );         //  75
    vec_gmodes.push_back( { 2, 420, 150, 4, 7 } );         //  76
    vec_gmodes.push_back( { 2, 420, 105, 4,10 } );         //  77
    vec_gmodes.push_back( { 2, 420,  75, 4,14 } );         //  78
    vec_gmodes.push_back( { 2, 420,  70, 4,15 } );         //  79
    vec_gmodes.push_back( { 2, 336, 525, 5, 2 } );         //  80
    vec_gmodes.push_back( { 2, 336, 350, 5, 3 } );         //  81
    vec_gmodes.push_back( { 2, 336, 210, 5, 5 } );         //  82
    vec_gmodes.push_back( { 2, 336, 175, 5, 6 } );         //  83
    vec_gmodes.push_back( { 2, 336, 150, 5, 7 } );         //  84
    vec_gmodes.push_back( { 2, 336, 105, 5,10 } );         //  85
    vec_gmodes.push_back( { 2, 336,  75, 5,14 } );         //  86
    vec_gmodes.push_back( { 2, 336,  70, 5,15 } );         //  87
    vec_gmodes.push_back( { 2, 210, 525, 8, 2 } );         //  88
    vec_gmodes.push_back( { 2, 210, 350, 8, 3 } );         //  89
    vec_gmodes.push_back( { 2, 210, 210, 8, 5 } );         //  90
    vec_gmodes.push_back( { 2, 210, 175, 8, 6 } );         //  91
    vec_gmodes.push_back( { 2, 210, 150, 8, 7 } );         //  92
    vec_gmodes.push_back( { 2, 210, 105, 8,10 } );         //  93
    vec_gmodes.push_back( { 2, 210,  75, 8,14 } );         //  94
    vec_gmodes.push_back( { 2, 210,  70, 8,15 } );         //  95

    // 1280 x 800
    vec_gmodes.push_back( { 3, 640, 800, 2, 1 } );         //  96
    vec_gmodes.push_back( { 3, 640, 400, 2, 2 } );         //  97
    vec_gmodes.push_back( { 3, 640, 266, 2, 3 } );         //  98
    vec_gmodes.push_back( { 3, 640, 200, 2, 4 } );         //  99
    vec_gmodes.push_back( { 3, 640, 160, 2, 5 } );         // 100
    vec_gmodes.push_back( { 3, 640, 133, 2, 6 } );         // 101
    vec_gmodes.push_back( { 3, 640, 114, 2, 7 } );         // 102
    vec_gmodes.push_back( { 3, 640, 100, 2, 8 } );         // 103
    vec_gmodes.push_back( { 3, 320, 800, 4, 1 } );         // 104
    vec_gmodes.push_back( { 3, 320, 400, 4, 2 } );         // 105
    vec_gmodes.push_back( { 3, 320, 266, 4, 3 } );         // 106
    vec_gmodes.push_back( { 3, 320, 200, 4, 4 } );         // 107
    vec_gmodes.push_back( { 3, 320, 160, 4, 5 } );         // 108
    vec_gmodes.push_back( { 3, 320, 133, 4, 6 } );         // 109
    vec_gmodes.push_back( { 3, 320, 114, 4, 7 } );         // 110
    vec_gmodes.push_back( { 3, 320, 100, 4, 8 } );         // 111
    vec_gmodes.push_back( { 3, 256, 800, 5, 1 } );         // 112
    vec_gmodes.push_back( { 3, 256, 400, 5, 2 } );         // 113
    vec_gmodes.push_back( { 3, 256, 266, 5, 3 } );         // 114
    vec_gmodes.push_back( { 3, 256, 200, 5, 4 } );         // 115
    vec_gmodes.push_back( { 3, 256, 160, 5, 5 } );         // 116
    vec_gmodes.push_back( { 3, 256, 133, 5, 6 } );         // 117
    vec_gmodes.push_back( { 3, 256, 114, 5, 7 } );         // 118
    vec_gmodes.push_back( { 3, 256, 100, 5, 8 } );         // 119
    vec_gmodes.push_back( { 3, 160, 800, 8, 1 } );         // 120
    vec_gmodes.push_back( { 3, 160, 400, 8, 2 } );         // 121
    vec_gmodes.push_back( { 3, 160, 266, 8, 3 } );         // 122
    vec_gmodes.push_back( { 3, 160, 200, 8, 4 } );         // 123
    vec_gmodes.push_back( { 3, 160, 160, 8, 5 } );         // 124
    vec_gmodes.push_back( { 3, 160, 133, 8, 6 } );         // 125
    vec_gmodes.push_back( { 3, 160, 114, 8, 7 } );         // 126
    vec_gmodes.push_back( { 3, 160, 100, 8, 8 } );         // 127

    // 1280 x 720
    vec_gmodes.push_back( { 4, 640, 720, 2, 1 } );         // 128
    vec_gmodes.push_back( { 4, 640, 360, 2, 2 } );         // 129
    vec_gmodes.push_back( { 4, 640, 240, 2, 3 } );         // 130
    vec_gmodes.push_back( { 4, 640, 180, 2, 4 } );         // 131
    vec_gmodes.push_back( { 4, 640, 144, 2, 5 } );         // 132
    vec_gmodes.push_back( { 4, 640, 120, 2, 6 } );         // 133
    vec_gmodes.push_back( { 4, 640, 102, 2, 7 } );         // 134
    vec_gmodes.push_back( { 4, 640,  90, 2, 8 } );         // 135
    vec_gmodes.push_back( { 4, 320, 720, 4, 1 } );         // 136
    vec_gmodes.push_back( { 4, 320, 360, 4, 2 } );         // 137
    vec_gmodes.push_back( { 4, 320, 240, 4, 3 } );         // 138
    vec_gmodes.push_back( { 4, 320, 180, 4, 4 } );         // 139
    vec_gmodes.push_back( { 4, 320, 144, 4, 5 } );         // 140
    vec_gmodes.push_back( { 4, 320, 120, 4, 6 } );         // 141
    vec_gmodes.push_back( { 4, 320, 102, 4, 7 } );         // 142
    vec_gmodes.push_back( { 4, 320,  90, 4, 8 } );         // 143
    vec_gmodes.push_back( { 4, 256, 720, 5, 1 } );         // 144
    vec_gmodes.push_back( { 4, 256, 360, 5, 2 } );         // 145
    vec_gmodes.push_back( { 4, 256, 240, 5, 3 } );         // 146
    vec_gmodes.push_back( { 4, 256, 180, 5, 4 } );         // 147
    vec_gmodes.push_back( { 4, 256, 144, 5, 5 } );         // 148
    vec_gmodes.push_back( { 4, 256, 120, 5, 6 } );         // 149
    vec_gmodes.push_back( { 4, 256, 102, 5, 7 } );         // 150
    vec_gmodes.push_back( { 4, 256,  90, 5, 8 } );         // 151
    vec_gmodes.push_back( { 4, 160, 720, 8, 1 } );         // 152
    vec_gmodes.push_back( { 4, 160, 360, 8, 2 } );         // 153
    vec_gmodes.push_back( { 4, 160, 240, 8, 3 } );         // 154
    vec_gmodes.push_back( { 4, 160, 180, 8, 4 } );         // 155
    vec_gmodes.push_back( { 4, 160, 144, 8, 5 } );         // 156
    vec_gmodes.push_back( { 4, 160, 120, 8, 6 } );         // 157
    vec_gmodes.push_back( { 4, 160, 102, 8, 7 } );         // 158
    vec_gmodes.push_back( { 4, 160,  90, 8, 8 } );         // 159

    // 1024 x 768
    vec_gmodes.push_back( { 5, 512, 768, 2, 1 } );         // 160
    vec_gmodes.push_back( { 5, 512, 384, 2, 2 } );         // 161
    vec_gmodes.push_back( { 5, 512, 256, 2, 3 } );         // 162
    vec_gmodes.push_back( { 5, 512, 192, 2, 4 } );         // 163
    vec_gmodes.push_back( { 5, 512, 153, 2, 5 } );         // 164
    vec_gmodes.push_back( { 5, 512, 128, 2, 6 } );         // 165
    vec_gmodes.push_back( { 5, 512, 109, 2, 7 } );         // 166
    vec_gmodes.push_back( { 5, 512,  96, 2, 8 } );         // 167
    vec_gmodes.push_back( { 5, 341, 768, 3, 1 } );         // 168
    vec_gmodes.push_back( { 5, 341, 384, 3, 2 } );         // 169
    vec_gmodes.push_back( { 5, 341, 256, 3, 3 } );         // 170
    vec_gmodes.push_back( { 5, 341, 192, 3, 4 } );         // 171
    vec_gmodes.push_back( { 5, 341, 153, 3, 5 } );         // 172
    vec_gmodes.push_back( { 5, 341, 128, 3, 6 } );         // 173
    vec_gmodes.push_back( { 5, 341, 109, 3, 7 } );         // 174
    vec_gmodes.push_back( { 5, 341,  96, 3, 8 } );         // 175
    vec_gmodes.push_back( { 5, 256, 768, 4, 1 } );         // 176
    vec_gmodes.push_back( { 5, 256, 384, 4, 2 } );         // 177
    vec_gmodes.push_back( { 5, 256, 256, 4, 3 } );         // 178
    vec_gmodes.push_back( { 5, 256, 192, 4, 4 } );         // 179
    vec_gmodes.push_back( { 5, 256, 153, 4, 5 } );         // 180
    vec_gmodes.push_back( { 5, 256, 128, 4, 6 } );         // 181
    vec_gmodes.push_back( { 5, 256, 109, 4, 7 } );         // 182
    vec_gmodes.push_back( { 5, 256,  96, 4, 8 } );         // 183
    vec_gmodes.push_back( { 5, 128, 768, 8, 1 } );         // 184
    vec_gmodes.push_back( { 5, 128, 384, 8, 2 } );         // 185
    vec_gmodes.push_back( { 5, 128, 256, 8, 3 } );         // 186
    vec_gmodes.push_back( { 5, 128, 192, 8, 4 } );         // 187
    vec_gmodes.push_back( { 5, 128, 153, 8, 5 } );         // 188
    vec_gmodes.push_back( { 5, 128, 128, 8, 6 } );         // 189
    vec_gmodes.push_back( { 5, 128, 109, 8, 7 } );         // 190
    vec_gmodes.push_back( { 5, 128,  96, 8, 8 } );         // 191

    // 800 x 600
    vec_gmodes.push_back( { 6, 400, 600, 2, 1 } );         // 192
    vec_gmodes.push_back( { 6, 400, 300, 2, 2 } );         // 193
    vec_gmodes.push_back( { 6, 400, 200, 2, 3 } );         // 194
    vec_gmodes.push_back( { 6, 400, 150, 2, 4 } );         // 195
    vec_gmodes.push_back( { 6, 400, 120, 2, 5 } );         // 196
    vec_gmodes.push_back( { 6, 400, 100, 2, 6 } );         // 197
    vec_gmodes.push_back( { 6, 400,  85, 2, 7 } );         // 198
    vec_gmodes.push_back( { 6, 400,  75, 2, 8 } );         // 199
    vec_gmodes.push_back( { 6, 200, 600, 4, 1 } );         // 200
    vec_gmodes.push_back( { 6, 200, 300, 4, 2 } );         // 201
    vec_gmodes.push_back( { 6, 200, 200, 4, 3 } );         // 202
    vec_gmodes.push_back( { 6, 200, 150, 4, 4 } );         // 203
    vec_gmodes.push_back( { 6, 200, 120, 4, 5 } );         // 204
    vec_gmodes.push_back( { 6, 200, 100, 4, 6 } );         // 205
    vec_gmodes.push_back( { 6, 200,  85, 4, 7 } );         // 206
    vec_gmodes.push_back( { 6, 200,  75, 4, 8 } );         // 207
    vec_gmodes.push_back( { 6, 160, 600, 5, 1 } );         // 208
    vec_gmodes.push_back( { 6, 160, 300, 5, 2 } );         // 209
    vec_gmodes.push_back( { 6, 160, 200, 5, 3 } );         // 210
    vec_gmodes.push_back( { 6, 160, 150, 5, 4 } );         // 211
    vec_gmodes.push_back( { 6, 160, 120, 5, 5 } );         // 212
    vec_gmodes.push_back( { 6, 160, 100, 5, 6 } );         // 213
    vec_gmodes.push_back( { 6, 160,  85, 5, 7 } );         // 214
    vec_gmodes.push_back( { 6, 160,  75, 5, 8 } );         // 215
    vec_gmodes.push_back( { 6, 100, 600, 8, 1 } );         // 216
    vec_gmodes.push_back( { 6, 100, 300, 8, 2 } );         // 217
    vec_gmodes.push_back( { 6, 100, 200, 8, 3 } );         // 218
    vec_gmodes.push_back( { 6, 100, 150, 8, 4 } );         // 219
    vec_gmodes.push_back( { 6, 100, 120, 8, 5 } );         // 220
    vec_gmodes.push_back( { 6, 100, 100, 8, 6 } );         // 221
    vec_gmodes.push_back( { 6, 100,  85, 8, 7 } );         // 222
    vec_gmodes.push_back( { 6, 100,  75, 8, 8 } );         // 223

    // 640 x 480
    vec_gmodes.push_back( { 7, 640, 480, 1, 1 } );         // 224
    vec_gmodes.push_back( { 7, 640, 240, 1, 2 } );         // 225
    vec_gmodes.push_back( { 7, 640, 160, 1, 3 } );         // 226
    vec_gmodes.push_back( { 7, 640, 120, 1, 4 } );         // 227
    vec_gmodes.push_back( { 7, 640,  96, 1, 5 } );         // 228
    vec_gmodes.push_back( { 7, 640,  80, 1, 6 } );         // 229
    vec_gmodes.push_back( { 7, 640,  68, 1, 7 } );         // 230
    vec_gmodes.push_back( { 7, 640,  60, 1, 8 } );         // 231
    vec_gmodes.push_back( { 7, 320, 480, 2, 1 } );         // 232
    vec_gmodes.push_back( { 7, 320, 240, 2, 2 } );         // 233
    vec_gmodes.push_back( { 7, 320, 160, 2, 3 } );         // 234
    vec_gmodes.push_back( { 7, 320, 120, 2, 4 } );         // 235
    vec_gmodes.push_back( { 7, 320,  96, 2, 5 } );         // 236
    vec_gmodes.push_back( { 7, 320,  80, 2, 6 } );         // 237
    vec_gmodes.push_back( { 7, 320,  68, 2, 7 } );         // 238
    vec_gmodes.push_back( { 7, 320,  60, 2, 8 } );         // 239
    vec_gmodes.push_back( { 7, 160, 480, 4, 1 } );         // 240
    vec_gmodes.push_back( { 7, 160, 240, 4, 2 } );         // 241
    vec_gmodes.push_back( { 7, 160, 160, 4, 3 } );         // 242
    vec_gmodes.push_back( { 7, 160, 120, 4, 4 } );         // 243
    vec_gmodes.push_back( { 7, 160,  96, 4, 5 } );         // 244
    vec_gmodes.push_back( { 7, 160,  80, 4, 6 } );         // 245
    vec_gmodes.push_back( { 7, 160,  68, 4, 7 } );         // 246
    vec_gmodes.push_back( { 7, 160,  60, 4, 8 } );         // 247
    vec_gmodes.push_back( { 7, 128, 480, 5, 1 } );         // 248
    vec_gmodes.push_back( { 7, 128, 240, 5, 2 } );         // 249
    vec_gmodes.push_back( { 7, 128, 160, 5, 3 } );         // 250
    vec_gmodes.push_back( { 7, 128, 120, 5, 4 } );         // 251
    vec_gmodes.push_back( { 7, 128,  96, 5, 5 } );         // 252
    vec_gmodes.push_back( { 7, 128,  80, 5, 6 } );         // 253
    vec_gmodes.push_back( { 7, 128,  68, 5, 7 } );         // 254
    vec_gmodes.push_back( { 7, 128,  60, 5, 8 } );         // 255

    *****/

    // Timings
    vec_timings.push_back( { 1280,  800 } );                //  0 = 1280 x 800
    vec_timings.push_back( { 1280,  720 } );                //  1 = 1280 x 720
    vec_timings.push_back( { 1024,  768 } );                //  2 = 1024 x 768
    vec_timings.push_back( {  640,  480 } );                //  3 =  640 x 480

    // timing index = (vec_gmode index) % 32

    // 1600 x 900
    vec_gmodes.push_back( { 0, 640, 400, 2, 2 } );         //   0



    // verify gmodes
    int i=0;
    for (auto& a: vec_gmodes)
    {
        Byte timing_index = a.Timing_index;     // or (i/32);
        printf("I:%3d  RW:%3d  RH:%3d  PW:%2d RH:%2d  %4dx%4d", i, 
            a.Res_Width, a.Res_Height, a.Pixel_Width, a.Pixel_Height, 
            a.Res_Width*a.Pixel_Width, a.Res_Height*a.Pixel_Height);
        printf("    Timing: %2d  DW:%4d  DH:%4d", timing_index,
            vec_timings[timing_index].Width,
            vec_timings[timing_index].Height );
        Word pw = vec_timings[timing_index].Width - (a.Res_Width*a.Pixel_Width);
        Word ph = vec_timings[timing_index].Height - (a.Res_Height*a.Pixel_Height);

        if (pw != 0 || ph != 0)
            printf("  PADDING: %2d x%2d", pw, ph);
        printf("\n");
        
        i++;
    }

}

