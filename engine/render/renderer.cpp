#include "renderer.h"

namespace engine
{
Renderer::Renderer(Window *window)
    : m_window(window)
    , m_currentShader(nullptr)
    , m_mesh(nullptr)
    , m_perViewConstantBuffer(nullptr)
    , m_perMeshConstantBuffer(nullptr)
{
    CreateConstantBuffers();
}

void Renderer::Update()
{
    PerViewConstantBuffer perViewData;
    DirectX::XMStoreFloat4x4(&perViewData.viewProjMatrix, m_camera->GetViewProjMatrix());
    UpdatePerViewConstantBuffer(perViewData);

    PerMeshConstantBuffer perMeshData;
    DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, m_mesh->m_modelMatrix);
    UpdatePerMeshConstantBuffer(perMeshData);
}

void Renderer::Render()
{
    Globals *globals = Globals::GetInstance();

    Update();

    m_window->SetViewport();
    m_window->ClearRenderTarget();
    m_window->SetRenderTarget();
    
    m_currentShader->Bind();

    globals->BindSamplers();

    BindConstantBuffers();

    m_mesh->m_texture->Bind(0);
    m_mesh->m_vertexBuffer->Bind();

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

    for (uint32_t i = 0; i != 1; ++i)
    {
        globals->m_deviceContext->VSSetConstantBuffers(i, 1, &nullBuffer);
        globals->m_deviceContext->PSSetConstantBuffers(i, 1, &nullBuffer);
    }

    m_perViewConstantBuffer->m_buffer.Reset();
    m_perMeshConstantBuffer->m_buffer.Reset();

    // Just break references to them, they will be destructed in Engine::Deinit():
    m_currentShader = nullptr;
    m_mesh = nullptr;
}

void Renderer::CreateConstantBuffers()
{
    m_perViewConstantBuffer = std::make_shared<Buffer>(
        nullptr,
        sizeof(PerViewConstantBuffer),
        sizeof(PerViewConstantBuffer),
        BufferUsage_ConstantBuffer);

    m_perMeshConstantBuffer = std::make_shared<Buffer>(
        nullptr,
        sizeof(PerMeshConstantBuffer),
        sizeof(PerMeshConstantBuffer),
        BufferUsage_ConstantBuffer);
}

void Renderer::BindConstantBuffers()
{
    m_perViewConstantBuffer->Bind(0);
    m_perMeshConstantBuffer->Bind(1);
}

void Renderer::UpdatePerViewConstantBuffer(PerViewConstantBuffer &data)
{
    m_perViewConstantBuffer->Update(static_cast<void *>(&data), sizeof(data));
}

void Renderer::UpdatePerMeshConstantBuffer(PerMeshConstantBuffer &data)
{
    m_perMeshConstantBuffer->Update(static_cast<void *>(&data), sizeof(data));
}
} // namespace engine
