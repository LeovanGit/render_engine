#include "terrain.h"

namespace engine
{
Terrain::Terrain(const std::wstring &pathToHeightMap,
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
        engine::ShaderStage_VertexShader |
        engine::ShaderStage_HullShader |
        engine::ShaderStage_DomainShader |
        engine::ShaderStage_PixelShader,
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
    // TODO
}
} // namespace engine