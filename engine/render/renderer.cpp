#include "renderer.h"

namespace engine
{
Renderer::Renderer(Window *window)
    : m_window(window)
    , m_currentShader(nullptr)
    , m_debugShader(nullptr)
    , m_perViewConstantBuffer(nullptr)
    , m_perMeshConstantBuffer(nullptr)
    , m_streamOutputBuffer(nullptr)
{
    CreateConstantBuffers();
}

void Renderer::Update()
{
    PerViewConstantBuffer perViewData;
    DirectX::XMStoreFloat4x4(&perViewData.viewProjMatrix, m_camera->GetViewProjMatrix());
    UpdatePerViewConstantBuffer(perViewData);
}

void Renderer::Render(bool debugMode)
{
    Globals *globals = Globals::GetInstance();

    Update();

    m_window->SetViewport();
    m_window->ClearRenderTarget();
    m_window->BindRenderTarget();

    RenderOpaque();

    if (debugMode) RenderDebug();

    m_window->Present();
}

void Renderer::RenderOpaque()
{
    Globals *globals = Globals::GetInstance();

    m_currentShader->Bind();

    globals->BindSamplers();

    globals->BindDepthStencilState();
    globals->BindRasterizerState();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    BindPerViewConstantBuffer();

    for (auto mesh : m_meshes)
    {
        PerMeshConstantBuffer perMeshData;
        DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, mesh->m_modelMatrix);
        UpdatePerMeshConstantBuffer(perMeshData);
        BindPerMeshConstantBuffer();

        mesh->m_texture->Bind(0);
        mesh->m_vertexBuffer->Bind();
        mesh->m_indexBuffer->Bind();

        globals->m_deviceContext->DrawIndexed(mesh->m_indexBuffer->m_size, 0, 0);
    }
}

void Renderer::RenderDebug()
{
    Globals *globals = Globals::GetInstance();

    for (auto mesh : m_meshes)
    {
        PerMeshConstantBuffer perMeshData;
        DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, mesh->m_modelMatrix);
        UpdatePerMeshConstantBuffer(perMeshData);
        BindPerMeshConstantBuffer();

        m_debugShaderSO->Bind();

        globals->BindRasterizerState();

        BindPerViewConstantBuffer();

        mesh->m_vertexBuffer->Bind();
        mesh->m_indexBuffer->Bind();

        m_streamOutputBuffer->Bind();

        globals->BindDepthStencilState2();

        // draw to SO buffer:
        globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        globals->m_deviceContext->DrawIndexed(mesh->m_indexBuffer->m_size, 0, 0);
        
        UINT offsets[] = { 0 };
        ComPtr<ID3D11Buffer> nullBuffer = nullptr;
        globals->m_deviceContext->SOSetTargets(1, &nullBuffer, offsets);

        // draw from SO buffer:
        globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        m_debugShader->Bind();

        uint32_t stride = 16; // float4
        uint32_t offset = 0;
        globals->m_deviceContext->IASetVertexBuffers(
            0,
            1,
            m_streamOutputBuffer->m_buffer.GetAddressOf(),
            &stride,
            &offset);

        globals->BindDepthStencilState();

        globals->m_deviceContext->Draw(72, 0);
    }
}

void Renderer::UnbindAll()
{
    Globals *globals = Globals::GetInstance();

    ID3D11RenderTargetView *nullRTV = nullptr;
    globals->m_deviceContext->OMSetRenderTargets(1, &nullRTV, nullptr);

    ID3D11InputLayout *nullInputLayout = nullptr;
    globals->m_deviceContext->IASetInputLayout(nullInputLayout);

    ID3D11VertexShader *nullVS = nullptr;
    globals->m_deviceContext->VSSetShader(nullVS, nullptr, 0);

    ID3D11GeometryShader *nullGS = nullptr;
    globals->m_deviceContext->GSSetShader(nullGS, nullptr, 0);

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

    globals->m_deviceContext->IASetIndexBuffer(
        nullBuffer,
        DXGI_FORMAT_R16_UINT,
        0);

    for (uint32_t i = 0; i != 1; ++i)
    {
        globals->m_deviceContext->VSSetConstantBuffers(i, 1, &nullBuffer);
        globals->m_deviceContext->PSSetConstantBuffers(i, 1, &nullBuffer);
        globals->m_deviceContext->GSSetConstantBuffers(i, 1, &nullBuffer);
    }

    globals->m_deviceContext->OMSetDepthStencilState(nullptr, 0);

    globals->m_deviceContext->RSSetState(nullptr);
}

void Renderer::Destroy()
{
    UnbindAll();

    m_streamOutputBuffer->m_buffer.Reset();

    m_perViewConstantBuffer->m_buffer.Reset();
    m_perMeshConstantBuffer->m_buffer.Reset();

    // Just break references to them, they will be destructed in Engine::Deinit():
    m_currentShader = nullptr;
    m_debugShader = nullptr;
    m_meshes.clear();
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

void Renderer::UpdatePerViewConstantBuffer(PerViewConstantBuffer &data)
{
    m_perViewConstantBuffer->Update(static_cast<void *>(&data), sizeof(data));
}

void Renderer::UpdatePerMeshConstantBuffer(PerMeshConstantBuffer &data)
{
    m_perMeshConstantBuffer->Update(static_cast<void *>(&data), sizeof(data));
}

void Renderer::BindPerViewConstantBuffer()
{
    m_perViewConstantBuffer->Bind(0);
}

void Renderer::BindPerMeshConstantBuffer()
{
    m_perMeshConstantBuffer->Bind(1);
}
} // namespace engine
