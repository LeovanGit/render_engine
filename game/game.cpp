#include <SDL.h>
#include <iostream>

namespace
{
constexpr uint32_t g_windowWidth = 640;
constexpr uint32_t g_windowHeight = 480;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        return 1;
    }

    return 0;
}
