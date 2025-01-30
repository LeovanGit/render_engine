#pragma once

#include <iostream>

#include <DirectXMath.h>

#include "buffer.h"
#include "texture_manager.h"

namespace engine
{
class Mesh
{
public:
    Mesh(const std::wstring &pathToTexture,
        void *data,
        size_t bufferSize,
        size_t vertexSize,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);
    ~Mesh() = default;

    std::shared_ptr<Buffer> m_vertexBuffer;
    std::shared_ptr<Texture> m_texture;
    DirectX::XMMATRIX m_modelMatrix;
};
} // namespace engine