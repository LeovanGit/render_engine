#pragma once

#include "window/window.h"
#include "vertex_buffer.h"
#include "shader_manager.h"

namespace engine
{
class Renderer
{
public:
    Renderer(Window *window);
    ~Renderer() = default;

    void Render();

    Window *m_window;
    std::shared_ptr<VertexBuffer> m_currentVertexBuffer;
    std::shared_ptr<Shader> m_currentShader;
};
} // namespace engine
