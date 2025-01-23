#include "vertex_buffer.h"

#include <cassert>

namespace engine
{
VertexBuffer::VertexBuffer(void *data, size_t bufferSize, size_t vertexSize)
{
    Globals *globals = Globals::GetInstance();

    D3D11_BUFFER_DESC bufferInfo = {};
    bufferInfo.ByteWidth = bufferSize;
    bufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
    bufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = data;

    HRESULT hr = globals->m_device->CreateBuffer(
        &bufferInfo,
        &resourceData,
        &m_buffer);
    assert(hr >= 0 && "Failed to create triangle vertex buffer\n");

    m_stride = vertexSize;
    m_offset = 0;
}

void VertexBuffer::Bind()
{
    Globals *globals = Globals::GetInstance();

    globals->m_deviceContext->IASetVertexBuffers(
        0,
        1,
        m_buffer.GetAddressOf(),
        &m_stride,
        &m_offset);
}
} // engine