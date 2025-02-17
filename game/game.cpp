#include <SDL.h>

#include "graphics_context/engine.h"
#include "timer/timer.h"

#include "controller.h"

int main(int argc, char *argv[])
{
    engine::Engine::Create();
    engine::Engine *engine = engine::Engine::GetInstance();
    engine->Init();

    Controller controller;
    controller.InitScene();

    engine::FPSTimer fpsTimer(0);
    fpsTimer.Start();

    bool keyNDown = false; // could be std::map with event.key.keysym.sym as keys

    // TODO: hide SDL under engine.vcxproj
    SDL_Event event;
    bool run = true;
    while (run)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
            {
                run = false;
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    controller.OnWindowResize(event.window.data1, event.window.data2);
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_n && !keyNDown)
                {
                    controller.m_drawDebug = !controller.m_drawDebug;
                    keyNDown = true;
                }
                break;
            }
            case SDL_KEYUP:
            {
                if (event.key.keysym.sym == SDLK_n && keyNDown)
                {
                    keyNDown = false;
                }
                break;
            }
            case SDL_MOUSEWHEEL:
            {
                float newMoveSpeed = controller.m_moveSpeed + event.wheel.y;
                controller.m_moveSpeed = newMoveSpeed > 0 ? newMoveSpeed : 0.1f;
                break;
            }
            }
        }
        
        if (fpsTimer.IsTimeElapsed())
        {
            controller.Update(fpsTimer.GetDeltaTime(), fpsTimer.GetFPS());
            controller.Draw();
        }
    }

    controller.Destroy();
    engine->Deinit();
    engine::Engine::Destroy();

    return 0;
}
