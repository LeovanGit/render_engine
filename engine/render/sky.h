#pragma once

#include "shader_manager.h"
#include "texture_manager.h"

namespace engine
{
class Sky
{
public:
    Sky(const std::wstring &pathToCubemap);
    ~Sky() = default;

    void Render();

    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture> m_cubemap;
};
} // namespace engine
