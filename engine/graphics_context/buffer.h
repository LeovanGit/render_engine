#pragma once

#include "utils/d3dcommon.h"
#include "globals.h"

#include <iostream>

namespace engine
{
enum BufferUsage
{
    BufferUsage_VertexBuffer = 0,
    BufferUsage_IndexBuffer,
    BufferUsage_InstanceBuffer,
    BufferUsage_ConstantBuffer,
    BufferUsage_UploadBuffer,
    //BufferUsage_ReadBuffer,
    //BufferUsage_ReadStructuredBuffer,
    //BufferUsage_RWBuffer,
    //BufferUsage_RWStructuredBuffer
};

class Buffer
{
public:
    Buffer(BufferUsage usage, void *data, uint32_t byteSize, uint32_t stride = 0);

    ~Buffer() = default;

    void Update(void *data, uint32_t dataSize);

    static void BindVertexBuffer(uint32_t slot, std::shared_ptr<Buffer> buffer);
    static void BindIndexBuffer(std::shared_ptr<Buffer> buffer);

    ComPtr<ID3D12Resource> m_buffer;
    BufferUsage m_usage;
    uint32_t m_byteSize;
    uint32_t m_stride; // size of one element

private:
    void CreateAndInitDefaultBuffer(void *initData, uint32_t byteSize);
    void CreateConstantBuffer(uint32_t byteSize);
    void CreateUploadBuffer(uint32_t byteSize);
};
} // engine