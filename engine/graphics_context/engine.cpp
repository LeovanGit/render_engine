#include "engine.h"

#include <cassert>

#include <SDL.h>

namespace engine
{
Engine *Engine::s_instance = nullptr;

void Engine::Create()
{
    assert(s_instance == nullptr && "Engine instance already created\n");
    s_instance = new Engine;
}

Engine *Engine::GetInstance()
{
    assert(s_instance && "Engine instance is not created\n");
    return s_instance;
}

void Engine::Destroy()
{
    assert(s_instance && "Engine instance is not created\n");
    delete s_instance;
    s_instance = nullptr;
}

void Engine::Init()
{
    assert(SDL_Init(SDL_INIT_VIDEO) == 0 && "Couldn't initialize SDL\n");
    //SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't initialize SDL: %s\n", SDL_GetError());
    
    Globals::Create();    
    ConstantBufferManager::Create();
    ShaderManager::Create();
    //TextureManager::Create();
    ModelManager::Create();
}

void Engine::Deinit()
{
    // WARNING: Renderer still keep alive current texture, vertex buffer, input layout and shaders
    // because  shared_ptr, so we should break references to them separately before Engine::Deinit()
    // in Renderer::Destroy():
    ModelManager::Destroy();
    //TextureManager::Destroy();
    ShaderManager::Destroy();
    ConstantBufferManager::Destroy();
    Globals::Destroy();

    SDL_Quit(); // SDL_Init
}
} // namespace engine