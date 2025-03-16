#include "sky.h"

namespace engine
{
Sky::Sky(const std::wstring &pathToCubemap)
{
    Globals *globals = Globals::GetInstance();

    // SHADER
    ShaderManager *sm = ShaderManager::GetInstance();

    std::shared_ptr<Shader> shader = sm->GetOrCreateShader(
        ShaderStage_VertexShader |
        ShaderStage_PixelShader,
        L"../assets/shaders/skybox.hlsl");

    // STATES
    D3D12_BLEND_DESC blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    D3D12_DEPTH_STENCIL_DESC depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; // reversed depth

    D3D12_RASTERIZER_DESC rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    // PIPELINE STATE OBJECT
    engine::PipelineManager *pm = engine::PipelineManager::GetInstance();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc = {};
    PSODesc.pRootSignature = globals->m_globalRootSignature.Get();

    PSODesc.VS =
    {
        shader->m_VSBytecode->GetBufferPointer(),
        shader->m_VSBytecode->GetBufferSize()
    };

    PSODesc.PS =
    {
        shader->m_PSBytecode->GetBufferPointer(),
        shader->m_PSBytecode->GetBufferSize()
    };

    PSODesc.BlendState = blendState;
    PSODesc.SampleMask = UINT_MAX;

    PSODesc.RasterizerState = rasterizerState;
    PSODesc.DepthStencilState = depthStencilState;

    PSODesc.InputLayout = shader->m_inputLayout; // nullptr
    PSODesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSODesc.NumRenderTargets = 1;
    PSODesc.RTVFormats[0] = s_backBufferFormat;
    PSODesc.DSVFormat = s_depthBufferFormat;
    PSODesc.SampleDesc.Count = 1;
    PSODesc.SampleDesc.Quality = 0;
    PSODesc.NodeMask = 0;
    PSODesc.CachedPSO = { nullptr, 0 };
    PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    m_PSO = pm->GetOrCreatePipeline("skybox", &PSODesc);

    // TEXTURE
    TextureManager *tm = TextureManager::GetInstance();
    m_cubemap = tm->GetOrCreateTexture(pathToCubemap, TextureUsage_SRV);
}

Sky::~Sky()
{
    // Just break references to them, they will be destructed in Engine::Deinit():
    m_cubemap = nullptr;
    m_PSO = nullptr;
}

void Sky::Render()
{
    Globals *globals = Globals::GetInstance();

    m_PSO->Bind();

    // bind texture
    globals->BindSRVDescriptor(m_cubemap->m_indexInHeap);
    
    globals->m_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    globals->m_commandList->DrawInstanced(3, 1, 0, 0);
}
} // namespace engine
