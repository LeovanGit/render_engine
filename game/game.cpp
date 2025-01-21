#include <SDL.h>

#include <iostream>
#include <cassert>

namespace
{
constexpr uint32_t g_windowWidth = 640;
constexpr uint32_t g_windowHeight = 480;
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "SDL + Direct3D11",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        g_windowWidth,
        g_windowHeight,
        SDL_WINDOW_SHOWN);
    assert(window && "Failed to create window");

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

    SDL_Event event;
    bool run = true;

    while (run)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT) run = false;

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    SDL_Log("up\n");
                    break;
                case SDLK_DOWN:
                    SDL_Log("down\n");
                    break;
                case SDLK_RIGHT:
                    SDL_Log("right\n");
                    break;
                case SDLK_LEFT:
                    SDL_Log("left\n");
                    break;
                default:
                    SDL_Log("unknown\n");
                    break;
                }
            }
        }

        // fill window with green color
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 255, 0));
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window); // SDL_CreateWindow
    SDL_Quit(); // SDL_Init

    return 0;
}
