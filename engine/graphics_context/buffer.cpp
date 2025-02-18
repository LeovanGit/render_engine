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
    , m_bufferSRV(nullptr)
    , m_bufferUAV(nullptr)
    , m_bindInfo({ false, 0, ShaderStage::ShaderStage_None })
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

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
    case BufferUsage_ReadBuffer:
    {
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;

        break;
    }
    case BufferUsage_ReadStructuredBuffer:
    {
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = stride;

        break;
    }
    case BufferUsage_RWBuffer:
    {
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;

        break;
    }
    case BufferUsage_RWStructuredBuffer:
    {
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = stride;

        break;
    }
    default:
        assert(false && "Unknown buffer usage\n");
    }
    
    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = data;
    
    hr = globals->m_device->CreateBuffer(
        &bufferDesc,
        data ? &resourceData : nullptr,
        m_buffer.GetAddressOf());
    assert(hr >= 0 && "Failed to create buffer\n");

    if (usage == BufferUsage_ReadBuffer || usage == BufferUsage_ReadStructuredBuffer)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        // TODO: now all buffers hardcoded to uint: Buffer<uint>
        srvDesc.Format = (usage == BufferUsage_ReadBuffer ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN);
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = byteSize / stride;

        hr = globals->m_device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_bufferSRV.GetAddressOf());
        assert(hr >= 0 && "Failed to create SRV\n");
    }
    else if (usage == BufferUsage_RWBuffer || usage == BufferUsage_RWStructuredBuffer)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = (usage == BufferUsage_RWBuffer ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN);
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = byteSize / stride;
        uavDesc.Buffer.Flags = 0;

        hr = globals->m_device->CreateUnorderedAccessView(m_buffer.Get(), &uavDesc, m_bufferUAV.GetAddressOf());
        assert(hr >= 0 && "Failed to create UAV\n");
    }
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

        if (stages & ShaderStage_ComputeShader)
            globals->m_deviceContext->CSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        break;
    }
    case BufferUsage_ReadBuffer:
    case BufferUsage_ReadStructuredBuffer:
    {
        assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, buffer will not be bound!");

        if (stages & ShaderStage_VertexShader)
            globals->m_deviceContext->VSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        if (stages & ShaderStage_HullShader)
            globals->m_deviceContext->HSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        if (stages & ShaderStage_DomainShader)
            globals->m_deviceContext->DSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        if (stages & ShaderStage_GeometryShader)
            globals->m_deviceContext->GSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        if (stages & ShaderStage_PixelShader)
            globals->m_deviceContext->PSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        if (stages & ShaderStage_ComputeShader)
            globals->m_deviceContext->CSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        break;
    }
    case BufferUsage_RWBuffer:
    case BufferUsage_RWStructuredBuffer:
    {
        assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, buffer will not be bound!");

        if (stages & ShaderStage_ComputeShader)
        {
            globals->m_deviceContext->CSSetUnorderedAccessViews(
                slot,
                1,
                m_bufferUAV.GetAddressOf(),
                nullptr);
        }
        else
        {
            globals->m_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
                D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
                nullptr,
                nullptr,
                1 + slot, // u0 in PS is used for RTV
                1,
                m_bufferUAV.GetAddressOf(),
                nullptr);
        }
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

        if (m_bindInfo.stages & ShaderStage_ComputeShader)
            globals->m_deviceContext->CSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        break;
    }
    case BufferUsage_ReadBuffer:
    case BufferUsage_ReadStructuredBuffer:
    {
        ID3D11ShaderResourceView *nullSRV = nullptr;

        if (m_bindInfo.stages & ShaderStage_VertexShader)
            globals->m_deviceContext->VSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        if (m_bindInfo.stages & ShaderStage_HullShader)
            globals->m_deviceContext->HSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        if (m_bindInfo.stages & ShaderStage_DomainShader)
            globals->m_deviceContext->DSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        if (m_bindInfo.stages & ShaderStage_GeometryShader)
            globals->m_deviceContext->GSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        if (m_bindInfo.stages & ShaderStage_PixelShader)
            globals->m_deviceContext->PSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        if (m_bindInfo.stages & ShaderStage_ComputeShader)
            globals->m_deviceContext->CSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        break;
    }
    case BufferUsage_RWBuffer:
    case BufferUsage_RWStructuredBuffer:
    {
        ID3D11UnorderedAccessView *nullUAV = nullptr;

        if (m_bindInfo.stages & ShaderStage_ComputeShader)
        {
            globals->m_deviceContext->CSSetUnorderedAccessViews(
                m_bindInfo.slot,
                1,
                &nullUAV,
                nullptr);
        }
        else
        {
            globals->m_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
                D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
                nullptr,
                nullptr,
                1 + m_bindInfo.slot, // u0 in PS is used for RTV
                1,
                &nullUAV,
                nullptr);
        }
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