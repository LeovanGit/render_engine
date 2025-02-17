#include "buffer.h"

#include <cassert>

namespace engine
{
Buffer::Buffer(void *data, uint32_t byteSize, uint32_t stride, BufferUsage usage)
    : m_usage(usage)
    , m_buffer(nullptr)
    , m_size(byteSize / stride)
    , m_stride(stride)
    , m_offset(0)
    , m_bindInfo({ false, 0, ShaderStage::ShaderStage_None })
{
    Globals *globals = Globals::GetInstance();

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = byteSize;
    
    switch (usage)
    {
    case BufferUsage_VertexBuffer:
    {
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
    
        break;
    }
    case BufferUsage_IndexBuffer:
    {
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
    
        break;
    }
    case BufferUsage_ConstantBuffer:
    {
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
    
        break;
    }
    default:
        assert(false && "Unknown buffer usage\n");
    }
    
    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = data;
    
    HRESULT hr = globals->m_device->CreateBuffer(
        &bufferDesc,
        data ? &resourceData : nullptr,
        m_buffer.GetAddressOf());
    assert(hr >= 0 && "Failed to create buffer\n");
}

// only for D3D11_USAGE_DYNAMIC and D3D11_CPU_ACCESS_WRITE
void Buffer::Update(void *data, uint32_t byteSize)
{
    Globals *globals = Globals::GetInstance();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    globals->m_deviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, data, byteSize);
    globals->m_deviceContext->Unmap(m_buffer.Get(), 0);
}

void Buffer::Bind(uint32_t slot, ShaderStage stages)
{
    Globals *globals = Globals::GetInstance();

    m_bindInfo.isBound = true;
    m_bindInfo.slot = slot;
    m_bindInfo.stages = stages;

    switch (m_usage)
    {
    case BufferUsage_VertexBuffer:
    {
        globals->m_deviceContext->IASetVertexBuffers(
            slot,
            1,
            m_buffer.GetAddressOf(),
            &m_stride,
            &m_offset);

        break;
    }
    case BufferUsage_IndexBuffer:
    {
        globals->m_deviceContext->IASetIndexBuffer(
            m_buffer.Get(),
            DXGI_FORMAT_R16_UINT,
            m_offset);

        break;
    }
    case BufferUsage_ConstantBuffer:
    {
        assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, buffer will not be bound!");

        if (stages & ShaderStage_VertexShader)
            globals->m_deviceContext->VSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        if (stages & ShaderStage_HullShader)
            globals->m_deviceContext->HSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        if (stages & ShaderStage_DomainShader)
            globals->m_deviceContext->DSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        if (stages & ShaderStage_GeometryShader)
            globals->m_deviceContext->GSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        if (stages & ShaderStage_PixelShader)
            globals->m_deviceContext->PSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        break;
    }
    default:
        assert(false && "Unknown buffer usage\n");
    }
}

void Buffer::Unbind()
{
    Globals *globals = Globals::GetInstance();

    assert(m_bindInfo.isBound && "buffer is not bound");

    ID3D11Buffer *nullBuffer = nullptr;
    uint32_t stride = 0;
    uint32_t offset = 0;

    switch (m_usage)
    {
    case BufferUsage_VertexBuffer:
    {
        globals->m_deviceContext->IASetVertexBuffers(
            m_bindInfo.slot,
            1,
            &nullBuffer,
            &stride,
            &offset);

        break;
    }
    case BufferUsage_IndexBuffer:
    {
        globals->m_deviceContext->IASetIndexBuffer(
            nullBuffer,
            DXGI_FORMAT_R16_UINT,
            offset);

        break;
    }
    case BufferUsage_ConstantBuffer:
    {
        if (m_bindInfo.stages & ShaderStage_VertexShader)
            globals->m_deviceContext->VSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        if (m_bindInfo.stages & ShaderStage_HullShader)
            globals->m_deviceContext->HSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        if (m_bindInfo.stages & ShaderStage_DomainShader)
            globals->m_deviceContext->DSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        if (m_bindInfo.stages & ShaderStage_GeometryShader)
            globals->m_deviceContext->GSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        if (m_bindInfo.stages & ShaderStage_PixelShader)
            globals->m_deviceContext->PSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        break;
    }
    default:
        assert(false && "Unknown buffer usage\n");
    }

    m_bindInfo.isBound = false;
    m_bindInfo.slot = 0;
    m_bindInfo.stages = ShaderStage::ShaderStage_None;
}
} // namespace engine