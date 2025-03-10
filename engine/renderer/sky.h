#pragma once

#include "graphics_context/pipeline_manager.h"
#include "graphics_context/texture_manager.h"
#include "graphics_context/shader_manager.h"

namespace engine
{
class Sky
{
public:
    Sky(const std::wstring &pathToCubemap);
    
    ~Sky();

    void Render();

    std::shared_ptr<Pipeline> m_PSO;
    std::shared_ptr<Texture> m_cubemap;
};
} // namespace engine
