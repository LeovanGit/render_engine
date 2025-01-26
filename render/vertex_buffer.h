#pragma once

#include "utils/d3d11common.h"
#include "globals.h"

#include <iostream>

namespace engine
{
class VertexBuffer
{
public:
    VertexBuffer(void *data, size_t bufferSize, size_t vertexSize);
    ~VertexBuffer() = default;

    void Bind();

    ComPtr<ID3D11Buffer> m_buffer;
    uint32_t m_stride; // size of one vertex (VSInput)
    uint32_t m_offset;
};
} // engine