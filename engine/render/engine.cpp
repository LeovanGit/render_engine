#include "engine.h"

namespace engine
{
Engine *Engine::m_instance = nullptr;

void Engine::Init()
{
    assert(!m_instance && "Engine::Init() was called twice!");

    m_instance = new Engine();

    Direct3D::Init();
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
    Direct3D::Deinit();

    delete m_instance;
    m_instance = nullptr;
}
} // namespace engine
