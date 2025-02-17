#include "mesh.h"

namespace engine
{
Mesh::Mesh(const std::wstring &pathToTexture,
    void *vertexBufferData,
    size_t vertexBufferByteSize,
    size_t vertexBufferStride, // one vertex size
    void *indexBufferData,
    size_t indexBufferByteSize,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    m_vertexBuffer = std::make_shared<Buffer>(
        vertexBufferData,
        vertexBufferByteSize,
        vertexBufferStride,
        BufferUsage::BufferUsage_VertexBuffer);

    m_indexBuffer = std::make_shared<Buffer>(
        indexBufferData,
        indexBufferByteSize,
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