#include "opaque_instances.h"

#include "graphics_context/constant_buffer_manager.h"

namespace engine
{
/*OpaqueInstances::OpaqueInstances()
    : m_shader(nullptr)
    , m_debugShader(nullptr)
{
    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "UV",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "TRANSFORM",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            2,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            3,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_INSTANCE_DATA,
            1
        },
    };

    engine::ShaderManager *sm = engine::ShaderManager::GetInstance();

    m_shader = sm->GetOrCreateShader(
        ShaderStage_VertexShader |
        ShaderStage_PixelShader,
        L"../assets/shaders/opaque.hlsl",
        inputLayout,
        _countof(inputLayout));

    m_debugShader = sm->GetOrCreateShader(
        ShaderStage_VertexShader |
        ShaderStage_GeometryShader |
        ShaderStage_PixelShader,
        L"../assets/shaders/debug.hlsl",
        inputLayout,
        _countof(inputLayout));
}

OpaqueInstances::~OpaqueInstances()
{
    // Just break references to them, they will be destructed in Engine::Deinit():
    m_meshes.clear();
    m_debugShader = nullptr;
    m_shader = nullptr;
}

void OpaqueInstances::AddInstance(std::shared_ptr<Mesh> mesh)
{
    m_meshes.push_back(mesh);

    if (m_instanceBuffer) m_instanceBuffer.reset();

    struct InstanceBufferData
    {
        DirectX::XMFLOAT4X4 modelMatrix;
    };

    m_instanceBuffer = std::make_shared<Buffer>(
        nullptr,
        sizeof(InstanceBufferData) * m_meshes.size(),
        sizeof(InstanceBufferData),
        BufferUsage_InstanceBuffer);

    std::vector<InstanceBufferData> data;
    for (auto mesh : m_meshes)
    {
        InstanceBufferData instanceData;
        DirectX::XMStoreFloat4x4(&instanceData.modelMatrix, mesh->m_modelMatrix);
       
        data.push_back(instanceData);
    }

    m_instanceBuffer->Update(data.data(), data.size() * sizeof(InstanceBufferData));
}

void OpaqueInstances::Render()
{
    Globals *globals = Globals::GetInstance();
    ConstantBufferManager *cbm = ConstantBufferManager::GetInstance();

    m_shader->Bind();

    m_meshes[0]->m_texture->Bind(0, TextureUsage_SRV, ShaderStage_PixelShader);
    m_meshes[0]->m_vertexBuffer->Bind(0);
    m_instanceBuffer->Bind(1);
    m_meshes[0]->m_indexBuffer->Bind();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    globals->m_deviceContext->DrawIndexedInstanced(
        m_meshes[0]->m_indexBuffer->m_size,
        m_instanceBuffer->m_size,
        0,
        0,
        0);

    m_meshes[0]->m_indexBuffer->Unbind();
    m_instanceBuffer->Unbind();
    m_meshes[0]->m_vertexBuffer->Unbind();
    m_meshes[0]->m_texture->Unbind();

    m_shader->Unbind();
}

void OpaqueInstances::RenderDebug()
{
    Globals *globals = Globals::GetInstance();
    ConstantBufferManager *cbm = ConstantBufferManager::GetInstance();

    m_debugShader->Bind();

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (auto mesh : m_meshes)
    {
        ConstantBufferManager::PerMesh perMeshData;
        DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, mesh->m_modelMatrix);
        
        cbm->UpdatePerMeshConstantBuffer(perMeshData);

        mesh->m_vertexBuffer->Bind(0);
        mesh->m_indexBuffer->Bind();

        globals->m_deviceContext->DrawIndexed(mesh->m_indexBuffer->m_size, 0, 0);

        mesh->m_indexBuffer->Unbind();
        mesh->m_vertexBuffer->Unbind();
    }

    m_debugShader->Unbind();
}*/
} // namespace engine
