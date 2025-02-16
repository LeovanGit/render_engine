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
    BufferUsage_ReadBuffer
};

class Buffer
{
public:
    Buffer(void *data, uint32_t dataSize, uint32_t stride, BufferUsage usage, bool isStructured = false)
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
        case BufferUsage_ReadBuffer:
        {
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

            if (isStructured)
            {
                bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                bufferDesc.StructureByteStride = stride;
            }

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

        if (usage == BufferUsage_ReadBuffer)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = isStructured ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_FLOAT;
            srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
            srvDesc.Buffer.NumElements = m_size;
            srvDesc.Buffer.FirstElement = 0;

            hr = globals->m_device->CreateShaderResourceView(
                m_buffer.Get(),
                &srvDesc,
                m_bufferSRV.GetAddressOf());
            assert(hr >= 0 && "Failed to create buffer SRV\n");

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = isStructured ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_FLOAT;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = m_size;
            uavDesc.Buffer.Flags = 0;

            hr = globals->m_device->CreateUnorderedAccessView(
                m_buffer.Get(),
                &uavDesc,
                m_bufferUAV.GetAddressOf());
            assert(hr >= 0 && "Failed to create buffer UAV\n");
        }
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
            globals->m_deviceContext->HSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
            globals->m_deviceContext->DSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
            globals->m_deviceContext->GSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
            globals->m_deviceContext->PSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

            break;
        }
        case BufferUsage_ReadBuffer:
        {
            //globals->m_deviceContext->CSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());
            globals->m_deviceContext->CSSetUnorderedAccessViews(slot, 1, m_bufferUAV.GetAddressOf(), nullptr);

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

    ComPtr<ID3D11ShaderResourceView> m_bufferSRV;
    ComPtr<ID3D11UnorderedAccessView> m_bufferUAV;
};
} // engine