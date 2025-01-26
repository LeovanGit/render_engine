#include "renderer.h"

namespace engine
{
Renderer::Renderer(Window *window)
    : m_window(window)
    , m_currentVertexBuffer(nullptr)
    , m_currentShader(nullptr)
{

}

void Renderer::Render()
{
    Globals *globals = Globals::GetInstance();

    m_window->SetViewport();
    m_window->ClearRenderTarget();
    m_window->SetRenderTarget();
    
    m_currentShader->Bind();

    // Model:
    globals->BindSamplers();
    m_currentTexture->Bind(0);
    m_currentVertexBuffer->Bind();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->m_deviceContext->Draw(4, 0);

    m_window->Present();
}
} // namespace engine
