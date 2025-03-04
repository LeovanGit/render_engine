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
    BufferUsage_ReadBuffer,
    BufferUsage_ReadStructuredBuffer,
    BufferUsage_RWBuffer,
    BufferUsage_RWStructuredBuffer
};

class Buffer
{
public:
    Buffer(void *data, uint32_t byteSize, uint32_t stride, BufferUsage usage);

    ~Buffer() = default;

    void Update(void *data, uint32_t dataSize);

    void Bind(uint32_t slot = 0, ShaderStage stages = ShaderStage::ShaderStage_None);

    void Unbind();

    ComPtr<ID3D12Resource> m_buffer;
    BufferUsage m_usage;
    uint32_t m_byteSize;
    uint32_t m_stride; // size of one element

    //ComPtr<ID3D11ShaderResourceView> m_bufferSRV;
    //ComPtr<ID3D11UnorderedAccessView> m_bufferUAV;

    struct BindInfo
    {
        bool isBound;
        uint32_t slot;
        ShaderStage stages;
    } m_bindInfo;

private:
    void CreateAndInitDefaultBuffer(void *initData, uint32_t byteSize);

    uint32_t CalcConstantBufferByteSize(uint32_t byteSize);
    void CreateConstantBuffer(uint32_t byteSize);
};
} // engine