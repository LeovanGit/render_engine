#include "shader_manager.h"

namespace engine
{
ShaderManager *ShaderManager::m_instance = nullptr;

void ShaderManager::Init()
{
    assert(!m_instance && "ShaderManager::Init() was called twice!");

    m_instance = new ShaderManager();
}

ShaderManager *ShaderManager::GetInstance()
{
    assert(m_instance && "ShaderManager singleton is not initialized!");

    return m_instance;
}

void ShaderManager::Deinit()
{
    assert(m_instance && "ShaderManager singleton is not initalized or ShaderManager::Deinit() was called twice!!");

    delete m_instance;
    m_instance = nullptr;
}

// also we need to create input layout for each shader somewhere (using device)
Shader *ShaderManager::GetShader(const std::string &path,
                                 const std::string &entry_point,
                                 const std::string &shader_model) 
{
    // firstly, search shader in unordered_map by path:
    auto shader = m_shaders.find(path); // std::unordered_map<std::string, Shader>::iterator
    if (shader != m_shaders.end()) return &shader->second;

    // if shader with this path doesn't exist, then create it:
    auto new_shader = m_shaders.try_emplace(
        path,
        Shader(std::wstring(path.begin(), path.end()), // convert string to wstring
                            entry_point,
                            shader_model));

    return &new_shader.first->second; // std::pair<iterator, bool>
}
} // namespace engine
