#pragma once

#include "graphics_context/buffer.h"

namespace engine
{
class Mesh
{
public:
    Mesh(
        void *vertexBufferData,
        size_t vertexBufferByteSize,
        size_t vertexBufferStride, // size of one vertex
        void *indexBufferData,
        size_t indexBufferByteSize);

    ~Mesh() = default;

    std::shared_ptr<Buffer> m_vertexBuffer;
    std::shared_ptr<Buffer> m_indexBuffer;
};
} // namespace engine
