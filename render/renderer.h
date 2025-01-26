#pragma once

#include "window/window.h"
#include "vertex_buffer.h"
#include "shader_manager.h"
#include "texture_manager.h"

namespace engine
{
class Renderer
{
public:
    Renderer(Window *window);
    ~Renderer() = default;

    void Render();

    Window *m_window;

    // Model:
    std::shared_ptr<Texture> m_currentTexture;
    std::shared_ptr<VertexBuffer> m_currentVertexBuffer;

    std::shared_ptr<Shader> m_currentShader;
};
} // namespace engine
