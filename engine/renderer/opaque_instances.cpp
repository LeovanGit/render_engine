#include "opaque_instances.h"

namespace engine
{
OpaqueInstances::OpaqueInstances()
    : m_PSO(nullptr)
    //, m_debugShader(nullptr)
{
    Globals *globals = Globals::GetInstance();

    // INPUT LAYOUT
    D3D12_INPUT_ELEMENT_DESC inputElements[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "UV",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "TRANSFORM",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            2,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            3,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
    };

    D3D12_INPUT_LAYOUT_DESC inputLayout = {};
    inputLayout.pInputElementDescs = inputElements;
    inputLayout.NumElements = _countof(inputElements);

    // SHADER
    engine::ShaderManager *sm = engine::ShaderManager::GetInstance();

    std::shared_ptr<Shader> shader = sm->GetOrCreateShader(
        ShaderStage_VertexShader |
        ShaderStage_PixelShader,
        L"../assets/shaders/opaque.hlsl",
        inputLayout);

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

    PSODesc.InputLayout = shader->m_inputLayout;
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

    m_PSO = pm->GetOrCreatePipeline("opaque", &PSODesc);




    //m_debugShader = sm->GetOrCreateShader(
    //    ShaderStage_VertexShader |
    //    ShaderStage_GeometryShader |
    //    ShaderStage_PixelShader,
    //    L"../assets/shaders/debug.hlsl",
    //    inputLayout,
    //    _countof(inputLayout));
}

OpaqueInstances::~OpaqueInstances()
{
    // Just break references to them, they will be destructed in Engine::Deinit():
    m_meshes.clear();
    //m_debugShader = nullptr;
    m_PSO = nullptr;
}

void OpaqueInstances::AddInstance(std::shared_ptr<Mesh> mesh)
{
    m_meshes.push_back(mesh);

    static uint32_t hackCounter = 0;
    if (hackCounter < 1)
    {
        hackCounter++;
        return;
    }

    if (m_instanceBuffer) m_instanceBuffer.reset();

    struct InstanceBufferData
    {
        DirectX::XMFLOAT4X4 modelMatrix;
    };

    std::vector<InstanceBufferData> data;
    for (auto mesh : m_meshes)
    {
        InstanceBufferData instanceData;
        DirectX::XMStoreFloat4x4(&instanceData.modelMatrix, mesh->m_modelMatrix);
       
        data.push_back(instanceData);
    }

    m_instanceBuffer = std::make_shared<Buffer>(
        BufferUsage_InstanceBuffer,
        data.data(),
        sizeof(InstanceBufferData) * m_meshes.size(),
        sizeof(InstanceBufferData));

    //m_instanceBuffer->Update(data.data(), data.size() * sizeof(InstanceBufferData));
}

void OpaqueInstances::Render()
{
    Globals *globals = Globals::GetInstance();

    m_PSO->Bind();

    Buffer::BindVertexBuffer(0, m_meshes[0]->m_vertexBuffer);
    Buffer::BindVertexBuffer(1, m_instanceBuffer);

    Buffer::BindIndexBuffer(m_meshes[0]->m_indexBuffer);

    globals->m_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->m_commandList->DrawIndexedInstanced(
        m_meshes[0]->m_indexBuffer->m_byteSize / m_meshes[0]->m_indexBuffer->m_stride,
        m_instanceBuffer->m_byteSize / m_instanceBuffer->m_stride,
        0,
        0,
        0);
}

//void OpaqueInstances::RenderDebug()
//{
//    Globals *globals = Globals::GetInstance();
//    ConstantBufferManager *cbm = ConstantBufferManager::GetInstance();
//
//    m_debugShader->Bind();
//
//    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//    for (auto mesh : m_meshes)
//    {
//        ConstantBufferManager::PerMesh perMeshData;
//        DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, mesh->m_modelMatrix);
//        
//        cbm->UpdatePerMeshConstantBuffer(perMeshData);
//
//        mesh->m_vertexBuffer->Bind(0);
//        mesh->m_indexBuffer->Bind();
//
//        globals->m_deviceContext->DrawIndexed(mesh->m_indexBuffer->m_size, 0, 0);
//
//        mesh->m_indexBuffer->Unbind();
//        mesh->m_vertexBuffer->Unbind();
//    }
//
//    m_debugShader->Unbind();
//}
} // namespace engine
