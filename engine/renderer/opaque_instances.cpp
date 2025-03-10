#include "opaque_instances.h"

namespace engine
{
OpaqueInstances::OpaqueInstances()
    : m_instanceBuffer(nullptr)
    , m_PSO(nullptr)
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
}

OpaqueInstances::~OpaqueInstances()
{
    // Just break references to them, they will be destructed in Engine::Deinit():
    m_PSO = nullptr;
    m_instanceBuffer = nullptr;
    m_meshes.clear();
}

void OpaqueInstances::AddInstance(
    const std::wstring &pathToTexture,
    std::shared_ptr<Mesh> mesh,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    engine::TextureManager *tm = engine::TextureManager::GetInstance();
    std::shared_ptr<Texture> texture = tm->GetOrCreateTexture(pathToTexture, TextureUsage_SRV);

    DirectX::XMMATRIX scalingMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(rotation.x),
        DirectX::XMConvertToRadians(rotation.y),
        DirectX::XMConvertToRadians(rotation.z));
    DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // Scale -> Rotate -> Translate
    DirectX::XMFLOAT4X4 modelMatrix;
    DirectX::XMStoreFloat4x4(&modelMatrix,
        XMMatrixMultiply(XMMatrixMultiply(scalingMat, rotationMat), translationMat));

    for (auto &_mesh : m_meshes)
    {
        // search the same mesh
        if (_mesh.m_mesh == mesh)
        {
            // search the same material
            for (auto &_material : _mesh.m_perMaterial)
            {
                if (_material.m_material.m_texture == texture)
                {
                    InstanceData instance;
                    instance.m_modelMatrix = modelMatrix;

                    _material.m_instances.push_back(std::move(instance));
                    goto END;
                }
            }

            // material not found -> add new:
            Material material;
            material.m_texture = texture;

            InstanceData instance;
            instance.m_modelMatrix = modelMatrix;

            PerMaterial perMat;
            perMat.m_material = material;
            perMat.m_instances.push_back(std::move(instance));

            _mesh.m_perMaterial.push_back(std::move(perMat));
            goto END;
        }
    }

    // mesh not found -> add new:
    {
        Material material;
        material.m_texture = texture;

        InstanceData instance;
        instance.m_modelMatrix = modelMatrix;

        PerMaterial perMat;
        perMat.m_material = material;
        perMat.m_instances.push_back(std::move(instance));

        PerMesh perMesh;
        perMesh.m_mesh = mesh;
        perMesh.m_perMaterial.push_back(std::move(perMat));

        m_meshes.push_back(perMesh);
    }

    END:

    // recreate instance buffer (TODO: move to separate func. No need to do this each time we add
    // new instance, do it at the end, when all instances added)
    if (m_instanceBuffer) m_instanceBuffer.reset();

    std::vector<InstanceData> data;
    for (auto &_mesh : m_meshes)
    {
        for (auto &_material : _mesh.m_perMaterial)
        {
            // TODO: just memcpy m_instances instead of iterate over them
            for (auto &_instance : _material.m_instances)
            {
                data.push_back(_instance);
            }
        }
    }

    m_instanceBuffer = std::make_shared<Buffer>(
        BufferUsage_InstanceBuffer,
        data.data(),
        sizeof(InstanceData) * data.size(),
        sizeof(InstanceData));
}

void OpaqueInstances::Render()
{
    Globals *globals = Globals::GetInstance();

    m_PSO->Bind();
    globals->m_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->BindSRVDescriptorsHeap();

    Buffer::BindVertexBuffer(1, m_instanceBuffer);

    uint32_t instancesRendered = 0;

    for (auto &_mesh : m_meshes)
    {
        Buffer::BindVertexBuffer(0, _mesh.m_mesh->m_vertexBuffer);
        Buffer::BindIndexBuffer(_mesh.m_mesh->m_indexBuffer);

        uint32_t indexCountPerInstance = _mesh.m_mesh->m_indexBuffer->m_byteSize / _mesh.m_mesh->m_indexBuffer->m_stride;

        for (auto &_material : _mesh.m_perMaterial)
        {
            // Bind Texture. Tmp hack with m_slotInHeap:
            globals->BindSRVDescriptor(_material.m_material.m_texture->m_slotInHeap);

            globals->m_commandList->DrawIndexedInstanced(
                indexCountPerInstance,
                _material.m_instances.size(),
                0,
                0,
                instancesRendered);

            instancesRendered += _material.m_instances.size();
        }
    }
}
} // namespace engine
