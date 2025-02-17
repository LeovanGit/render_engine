#pragma once

#include "graphics_context/shader_manager.h"
#include "graphics_context/texture_manager.h"

namespace engine
{
class Sky
{
public:
    Sky(const std::wstring &pathToCubemap);
    
    ~Sky();

    void Render();

    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture> m_cubemap;
};
} // namespace engine
