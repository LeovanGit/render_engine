#include "model.h"

namespace engine
{
Mesh::Mesh(const std::wstring &pathToTexture,
    void *data,
    size_t bufferSize,
    size_t vertexSize,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 rotation)
{
    m_vertexBuffer = std::make_shared<Buffer>(
        data,
        bufferSize,
        vertexSize,
        BufferUsage::BufferUsage_VertexBuffer);

    engine::TextureManager *tm = engine::TextureManager::GetInstance();
    m_texture = tm->GetOrCreateTexture(pathToTexture);

    DirectX::XMMATRIX scalingMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(rotation.z, rotation.x, rotation.y);
    DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // Scale -> Rotate -> Translate
    m_modelMatrix = XMMatrixMultiply(translationMat, XMMatrixMultiply(scalingMat, rotationMat));
}
} // namespace engine