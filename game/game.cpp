#include <SDL.h>

#include <iostream>
#include <cassert>

#include "render/engine.h"
#include "window/window.h"

int main(int argc, char *argv[])
{
    engine::Engine::Create();
    engine::Engine *engine = engine::Engine::GetInstance();

    engine->Init();

    engine::Window window(640, 480);
    window.Create();

    // TODO: hide SDL under engine.vcxproj
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

        window.Update();
    }

    window.Destroy();
    engine->Deinit();
    engine::Engine::Destroy();

    return 0;
}
