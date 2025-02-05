#pragma once

#include "utils/d3d11common.h"
#include "globals.h"

#include <iostream>
#include <cassert>

namespace engine
{
enum BufferUsage
{
    BufferUsage_VertexBuffer = 0,
    BufferUsage_IndexBuffer,
    BufferUsage_ConstantBuffer,
    BufferUsage_StreamOutput
};

class Buffer
{
public:
    Buffer(void *data, uint32_t dataSize, uint32_t stride, BufferUsage usage)
        : m_usage(usage)
        , m_buffer(nullptr)
        , m_size(0)
        , m_stride(stride)
        , m_offset(0)
    {
        Globals *globals = Globals::GetInstance();

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.ByteWidth = dataSize;

        switch (usage)
        {
        case BufferUsage_VertexBuffer:
        {
            bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
            bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;

            break;
        }
        case BufferUsage_IndexBuffer:
        {
            bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
            bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;

            break;
        }
        case BufferUsage_ConstantBuffer:
        {
            bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
            bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            break;
        }
        case BufferUsage_StreamOutput:
        {
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
            bufferDesc.CPUAccessFlags = 0;
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

        m_size = dataSize / stride;
    }

    ~Buffer() = default;

    void Update(void *data, uint32_t dataSize)
    {
        Globals *globals = Globals::GetInstance();

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        globals->m_deviceContext->Map(m_buffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        memcpy(mappedResource.pData, data, dataSize);
        globals->m_deviceContext->Unmap(m_buffer.Get(), 0);
    }

    void Bind(uint32_t slot = 0)
    {
        Globals *globals = Globals::GetInstance();

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
            globals->m_deviceContext->VSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
            globals->m_deviceContext->PSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
            globals->m_deviceContext->GSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

            break;
        }
        case BufferUsage_StreamOutput:
        {
            UINT offsets[] = { 0 };
            globals->m_deviceContext->SOSetTargets(1, m_buffer.GetAddressOf(), offsets);

            break;
        }
        default:
            assert(false && "Unknown buffer usage\n");
        }
    }

    BufferUsage m_usage;
    ComPtr<ID3D11Buffer> m_buffer;
    uint32_t m_size;
    uint32_t m_stride; // size of one vertex (VSInput struct)
    uint32_t m_offset;
};
} // engine