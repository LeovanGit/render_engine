#include "model.h"

#include <memory>

namespace engine
{
Mesh::Mesh(
    void *vertexBufferData,
    size_t vertexBufferByteSize,
    size_t vertexBufferStride, // size of one vertex
    void *indexBufferData,
    size_t indexBufferByteSize)
{
    m_vertexBuffer = std::make_shared<Buffer>(
        BufferUsage::BufferUsage_VertexBuffer,
        vertexBufferData,
        vertexBufferByteSize,
        vertexBufferStride);

    m_indexBuffer = std::make_shared<Buffer>(
        BufferUsage::BufferUsage_IndexBuffer,
        indexBufferData,
        indexBufferByteSize,
        sizeof(uint16_t));
}
} // namespace engine
