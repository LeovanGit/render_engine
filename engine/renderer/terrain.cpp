#include "terrain.h"

#include "graphics_context/constant_buffer_manager.h"

namespace engine
{
/*Terrain::Terrain(const std::wstring &pathToHeightMap,
    uint32_t rows,
    uint32_t cols,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    ShaderManager *sm = ShaderManager::GetInstance();

    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT, // float3
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "UV",
            0,
            DXGI_FORMAT_R32G32_FLOAT, // float2
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    m_shader = sm->GetOrCreateShader(
        ShaderStage_VertexShader |
        ShaderStage_HullShader |
        ShaderStage_DomainShader |
        ShaderStage_PixelShader,
        L"../assets/shaders/terrain.hlsl",
        inputLayout,
        _countof(inputLayout));

    ModelManager *mm = ModelManager::GetInstance();

    m_mesh = mm->GeneratePlanesGrid(
        "terrain",
        pathToHeightMap,
        rows,
        cols,
        position,
        scale,
        rotation);
}

Terrain::~Terrain()
{
    // Just break references to them, they will be destructed in Engine::Deinit():
    m_mesh = nullptr;
    m_shader = nullptr;
}

void Terrain::Render()
{
    Globals *globals = Globals::GetInstance();
    ConstantBufferManager *cbm = ConstantBufferManager::GetInstance();

    ConstantBufferManager::PerMesh perMeshData;
    DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, m_mesh->m_modelMatrix);

    cbm->UpdatePerMeshConstantBuffer(perMeshData);
    
    m_shader->Bind();

    m_mesh->m_texture->Bind(0, TextureUsage_SRV, ShaderStage_DomainShader);
    m_mesh->m_vertexBuffer->Bind(0);
    m_mesh->m_indexBuffer->Bind();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    globals->m_deviceContext->DrawIndexed(m_mesh->m_indexBuffer->m_size, 0, 0);

    m_mesh->m_indexBuffer->Unbind();
    m_mesh->m_vertexBuffer->Unbind();
    m_mesh->m_texture->Unbind();

    m_shader->Unbind();
}*/
} // namespace engine