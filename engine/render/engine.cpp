#include "engine.h"

#include <cassert>

#include <SDL.h>

namespace engine
{
Engine *Engine::s_instance = nullptr;

void Engine::Create()
{
    assert(s_instance == nullptr && "Engine instance already created");
    s_instance = new Engine;
}

Engine *Engine::GetInstance()
{
    assert(s_instance && "Engine instance is not created");
    return s_instance;
}

void Engine::Destroy()
{
    assert(s_instance && "Engine instance is not created");
    delete s_instance;
    s_instance = nullptr;
}

void Engine::Init()
{
    assert(SDL_Init(SDL_INIT_VIDEO) == 0 && "Couldn't initialize SDL");
    //SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't initialize SDL: %s\n", SDL_GetError());
}

void Engine::Deinit()
{
    SDL_Quit(); // SDL_Init
}
} // namespace engine