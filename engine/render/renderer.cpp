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

    globals->m_deviceContext->Draw(6, 0);

    m_window->Present();
}

void Renderer::Destroy()
{
    Globals *globals = Globals::GetInstance();

    // unbind all resources
    ID3D11RenderTargetView *nullRTV = nullptr;
    globals->m_deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);

    ID3D11InputLayout *nullInputLayout = nullptr;
    globals->m_deviceContext->IASetInputLayout(nullInputLayout);

    ID3D11VertexShader *nullVS = nullptr;
    globals->m_deviceContext->VSSetShader(nullVS, nullptr, 0);

    ID3D11PixelShader *nullPS = nullptr;
    globals->m_deviceContext->PSSetShader(nullPS, nullptr, 0);

    ID3D11SamplerState *nullSampler = nullptr;
    globals->m_deviceContext->PSSetSamplers(0, 1, &nullSampler);

    ID3D11ShaderResourceView *nullSRV = nullptr;
    globals->m_deviceContext->PSSetShaderResources(0, 1, &nullSRV);

    ID3D11Buffer *nullBuffer = nullptr;
    UINT stride = 0;
    UINT offset = 0;
    globals->m_deviceContext->IASetVertexBuffers(
        0,
        1,
        &nullBuffer,
        &stride,
        &offset);

    // Just break references to them, they will be destructed in Engine::Deinit():
    m_currentTexture = nullptr;
    m_currentVertexBuffer = nullptr;
    m_currentShader = nullptr;
}
} // namespace engine
