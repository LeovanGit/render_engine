#pragma once

#include <iostream>

#include <DirectXMath.h>

#include "graphics_context/buffer.h"
#include "graphics_context/texture_manager.h"

namespace engine
{
class Mesh
{
public:
    Mesh(const std::wstring &pathToTexture,
        void *vertexBufferData,
        size_t vertexBufferByteSize,
        size_t vertexBufferStride, // size of one vertex
        void *indexBufferData,
        size_t indexBufferByteSize,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);

    ~Mesh() = default;

    std::shared_ptr<Texture> m_texture;

    std::shared_ptr<Buffer> m_vertexBuffer;
    std::shared_ptr<Buffer> m_indexBuffer;

    DirectX::XMMATRIX m_modelMatrix;
};
} // namespace engine
