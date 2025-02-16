#include "renderer.h"

namespace engine
{
Renderer::Renderer(Window *window)
    : m_window(window)
    , m_currentShader(nullptr)
    , m_debugShader(nullptr)
    , m_perViewConstantBuffer(nullptr)
    , m_perMeshConstantBuffer(nullptr)
    , m_terrain(nullptr)
{
    CreateConstantBuffers();
}

void Renderer::Update()
{
    PerViewConstantBuffer perViewData;
    DirectX::XMStoreFloat4x4(&perViewData.viewProjMatrix, m_camera->GetViewProjMatrix());
    DirectX::XMStoreFloat3(&perViewData.cameraPostionWS, m_camera->GetPosition());
    // camera frustum near plane's corners in WS:
    DirectX::XMStoreFloat3(&perViewData.nearPlaneTLCornerWS, m_camera->Reproject(-1.0f, 1.0f));
    DirectX::XMStoreFloat3(&perViewData.nearPlaneBLCornerWS, m_camera->Reproject(-1.0f, -1.0f));
    DirectX::XMStoreFloat3(&perViewData.nearPlaneBRCornerWS, m_camera->Reproject(1.0f, -1.0f));
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
    RenderTerrain();
    if (debugMode) RenderDebug();
    RenderSkybox();

    RenderComputeShader();

    m_window->Present();
}

void Renderer::RenderOpaque()
{
    Globals *globals = Globals::GetInstance();

    m_currentShader->Bind();

    globals->BindSamplers();

    globals->BindDepthStencilState();
    globals->BindRasterizerState();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

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

    m_debugShader->Bind();

    globals->BindDepthStencilState();
    globals->BindRasterizerState();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    BindPerViewConstantBuffer();

    for (auto mesh : m_meshes)
    {
        PerMeshConstantBuffer perMeshData;
        DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, mesh->m_modelMatrix);
        UpdatePerMeshConstantBuffer(perMeshData);
        BindPerMeshConstantBuffer();

        mesh->m_vertexBuffer->Bind();
        mesh->m_indexBuffer->Bind();

        globals->m_deviceContext->DrawIndexed(mesh->m_indexBuffer->m_size, 0, 0);
    }
}

void Renderer::RenderTerrain()
{
    Globals *globals = Globals::GetInstance();

    m_terrain->m_shader->Bind();

    BindPerViewConstantBuffer();

    PerMeshConstantBuffer perMeshData;
    DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, m_terrain->m_mesh->m_modelMatrix);
    UpdatePerMeshConstantBuffer(perMeshData);
    BindPerMeshConstantBuffer();

    m_terrain->m_mesh->m_texture->Bind(0);
    m_terrain->m_mesh->m_vertexBuffer->Bind();
    m_terrain->m_mesh->m_indexBuffer->Bind();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

    globals->m_deviceContext->DrawIndexed(m_terrain->m_mesh->m_indexBuffer->m_size, 0, 0);
}

void Renderer::RenderSkybox()
{
    Globals *globals = Globals::GetInstance();
    m_skybox->m_shader->Bind();
    
    globals->BindSamplers();

    ID3D11InputLayout *nullInputLayout = nullptr;
    globals->m_deviceContext->IASetInputLayout(nullInputLayout);

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

    BindPerViewConstantBuffer();

    m_skybox->m_cubemap->Bind(0);

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->m_deviceContext->Draw(3, 0);
}

void Renderer::RenderComputeShader()
{
    Globals *globals = Globals::GetInstance();
    m_computeShader->Bind();

    m_computeTexture->Bind(0);

    m_readBuffer->Bind(0);
    m_readStructuredBuffer->Bind(1);

    globals->m_deviceContext->Dispatch(160, 90, 1);
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

    ID3D11ComputeShader *nullCS = nullptr;
    globals->m_deviceContext->CSSetShader(nullCS, nullptr, 0);

    ID3D11SamplerState *nullSampler = nullptr;
    globals->m_deviceContext->PSSetSamplers(0, 1, &nullSampler);

    ID3D11ShaderResourceView *nullSRV = nullptr;
    globals->m_deviceContext->PSSetShaderResources(0, 1, &nullSRV);
    globals->m_deviceContext->DSSetShaderResources(0, 1, &nullSRV);
    globals->m_deviceContext->CSSetShaderResources(0, 1, &nullSRV);
    globals->m_deviceContext->CSSetShaderResources(1, 1, &nullSRV);

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

    m_perViewConstantBuffer->m_buffer.Reset();
    m_perMeshConstantBuffer->m_buffer.Reset();

    // Just break references to them, they will be destructed in Engine::Deinit():
    m_currentShader = nullptr;
    m_debugShader = nullptr;
    m_computeShader = nullptr;
    m_computeTexture = nullptr;
    m_readBuffer = nullptr;
    m_readStructuredBuffer = nullptr;
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
