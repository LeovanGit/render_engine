#include "engine.h"

namespace engine
{
Engine *Engine::m_instance = nullptr;

void Engine::Init()
{
    assert(!m_instance && "Engine::Init() was called twice!");

    m_instance = new Engine();

    m_instance->LoadEngineSettings();
    Direct3D::Init();
    ShaderManager::Init();
}

const Engine *Engine::GetInstance()
{
    assert(m_instance && "Engine singleton is not initalized!");

    return m_instance;
}

void Engine::Deinit()
{
    assert(m_instance && "Engine singleton is not initalized or Engine::Deinit() was called twice!");
    
    // deinitialize singletons in reverse order:
    ShaderManager::Deinit();
    Direct3D::Deinit();

    delete m_instance;
    m_instance = nullptr;
}

// TODO: load from some .ini file?
void Engine::LoadEngineSettings()
{
    settings.backbuffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    settings.swapchain_buffer_count = 2;
}
} // namespace engine
