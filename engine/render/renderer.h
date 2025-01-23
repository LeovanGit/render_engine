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
    ~Renderer();

    void SetVertexBuffer(VertexBuffer *buffer) { m_currentVertexBuffer = buffer; }
    void SetVertexShader(Shader *shader) { m_currentVertexShader = shader; }
    void SetPixelShader(Shader *shader) { m_currentPixelShader = shader; }

    void Render();

    Window *m_window;
    VertexBuffer *m_currentVertexBuffer;
    Shader *m_currentVertexShader;
    Shader *m_currentPixelShader;
};
} // namespace engine
