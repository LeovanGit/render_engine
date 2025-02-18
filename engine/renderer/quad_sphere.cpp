#include "quad_sphere.h"

namespace engine
{
QuadSphere::QuadSphere(
    const std::wstring &pathToTexture,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
    : m_shader(nullptr)
    , m_texture(nullptr)
{
    ShaderManager *sm = ShaderManager::GetInstance();

    m_shader = sm->GetOrCreateShader(
        ShaderStage_VertexShader |
        ShaderStage_HullShader |
        ShaderStage_DomainShader |
        ShaderStage_PixelShader,
        L"../assets/shaders/quad_sphere.hlsl",
        nullptr,
        0);

    TextureManager *tm = TextureManager::GetInstance();
    m_texture = tm->GetOrCreateTexture(pathToTexture, TextureUsage_SRV);

    DirectX::XMMATRIX scalingMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(rotation.x),
        DirectX::XMConvertToRadians(rotation.y),
        DirectX::XMConvertToRadians(rotation.z));
    DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // Scale -> Rotate -> Translate
    m_modelMatrix = XMMatrixMultiply(XMMatrixMultiply(scalingMat, rotationMat), translationMat);
}

QuadSphere::~QuadSphere()
{
    m_texture = nullptr;
    m_shader = nullptr;
}

void QuadSphere::Render()
{
    Globals *globals = Globals::GetInstance();
    ConstantBufferManager *cbm = ConstantBufferManager::GetInstance();

    ConstantBufferManager::PerMesh perMeshData;
    DirectX::XMStoreFloat4x4(&perMeshData.modelMatrix, m_modelMatrix);

    cbm->UpdatePerMeshConstantBuffer(perMeshData);

    m_shader->Bind();
    m_texture->Bind(0, TextureUsage_SRV, ShaderStage_PixelShader);

    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
    globals->m_deviceContext->Draw(24, 0); // cube is 6 quad patches

    m_texture->Unbind();
    m_shader->Unbind();
}
} // namespace engine
