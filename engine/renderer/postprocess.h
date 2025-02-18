#pragma once

#include "graphics_context/shader_manager.h"
#include "graphics_context/texture_manager.h"
#include "graphics_context/buffer.h"

namespace engine
{
class PostProcess
{
public:
    PostProcess();

    ~PostProcess();

    void Render();

    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture> m_RWTexture;
    std::shared_ptr<Buffer> m_readBuffer;
    std::shared_ptr<Buffer> m_readStructuredBuffer;
    std::shared_ptr<Buffer> m_RWBuffer;
    std::shared_ptr<Buffer> m_RWStructuredBuffer;
};
} // namespace engine
