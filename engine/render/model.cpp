#include "model.h"

namespace engine
{
Mesh::Mesh(const std::wstring &pathToTexture,
    void *vertexBufferData,
    size_t vertexBufferSize,
    size_t vertexSize,
    void *indexBufferData,
    size_t indexBufferSize,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    m_vertexBuffer = std::make_shared<Buffer>(
        vertexBufferData,
        vertexBufferSize,
        vertexSize,
        BufferUsage::BufferUsage_VertexBuffer);

    m_indexBuffer = std::make_shared<Buffer>(
        indexBufferData,
        indexBufferSize,
        sizeof(uint16_t),
        BufferUsage::BufferUsage_IndexBuffer);

    engine::TextureManager *tm = engine::TextureManager::GetInstance();
    m_texture = tm->GetOrCreateTexture(pathToTexture);

    DirectX::XMMATRIX scalingMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(rotation.x),
        DirectX::XMConvertToRadians(rotation.y),
        DirectX::XMConvertToRadians(rotation.z));
    DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // Scale -> Rotate -> Translate
    m_modelMatrix = XMMatrixMultiply(XMMatrixMultiply(scalingMat, rotationMat), translationMat);
}
} // namespace engine