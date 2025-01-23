#include "renderer.h"

namespace engine
{
Renderer::Renderer(Window *window)
    : m_window(window)
    , m_currentVertexBuffer(nullptr)
    , m_currentVertexShader(nullptr)
    , m_currentPixelShader(nullptr)
{

}

Renderer::~Renderer()
{
    if (m_currentVertexBuffer)
    {
        delete m_currentVertexBuffer;
        m_currentVertexBuffer = nullptr;
    }

    if (m_currentVertexShader)
    {
        delete m_currentVertexShader;
        m_currentVertexShader = nullptr;
    }

    if (m_currentPixelShader)
    {
        delete m_currentPixelShader;
        m_currentPixelShader = nullptr;
    }
}

void Renderer::Render()
{
    Globals *globals = Globals::GetInstance();

    m_window->SetViewport();
    m_window->ClearRenderTarget();
    m_window->SetRenderTarget();
    
    m_currentVertexShader->Bind();
    m_currentPixelShader->Bind();

    m_currentVertexBuffer->Bind();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->m_deviceContext->Draw(3, 0);

    m_window->Present();
}
} // namespace engine
