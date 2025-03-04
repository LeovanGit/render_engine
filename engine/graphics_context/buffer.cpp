#include "buffer.h"

#include <cassert>

namespace engine
{
Buffer::Buffer(void *data, uint32_t byteSize, uint32_t stride, BufferUsage usage)
    : m_buffer(nullptr)
    , m_usage(usage)
    , m_byteSize(byteSize)
    , m_stride(stride)
    //, m_bufferSRV(nullptr)
    //, m_bufferUAV(nullptr)
    , m_bindInfo({ false, 0, ShaderStage::ShaderStage_None })
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    switch (usage)
    {
    case BufferUsage_VertexBuffer:
    case BufferUsage_IndexBuffer:
    case BufferUsage_InstanceBuffer:
    {
        CreateAndInitDefaultBuffer(data, byteSize);
    
        break;
    }
    case BufferUsage_ConstantBuffer:
    {
        CreateConstantBuffer(byteSize);

        break;
    }
    case BufferUsage_ReadBuffer:
    {
        //bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        //bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        //bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        //bufferDesc.MiscFlags = 0;

        break;
    }
    case BufferUsage_ReadStructuredBuffer:
    {
        //bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        //bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        //bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        //bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //bufferDesc.StructureByteStride = stride;

        break;
    }
    case BufferUsage_RWBuffer:
    {
        //bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        //bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        //bufferDesc.CPUAccessFlags = 0;
        //bufferDesc.MiscFlags = 0;

        break;
    }
    case BufferUsage_RWStructuredBuffer:
    {
        //bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        //bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        //bufferDesc.CPUAccessFlags = 0;
        //bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //bufferDesc.StructureByteStride = stride;

        break;
    }
    default:
        assert(false && "Unknown buffer usage\n");
    }

    //if (usage == BufferUsage_ReadBuffer || usage == BufferUsage_ReadStructuredBuffer)
    //{
    //    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    //    // TODO: now all buffers hardcoded to uint: Buffer<uint>
    //    srvDesc.Format = (usage == BufferUsage_ReadBuffer ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN);
    //    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    //    srvDesc.Buffer.FirstElement = 0;
    //    srvDesc.Buffer.NumElements = byteSize / stride;

    //    hr = globals->m_device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_bufferSRV.GetAddressOf());
    //    assert(hr >= 0 && "Failed to create SRV\n");
    //}
    //else if (usage == BufferUsage_RWBuffer || usage == BufferUsage_RWStructuredBuffer)
    //{
    //    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    //    uavDesc.Format = (usage == BufferUsage_RWBuffer ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN);
    //    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    //    uavDesc.Buffer.FirstElement = 0;
    //    uavDesc.Buffer.NumElements = byteSize / stride;
    //    uavDesc.Buffer.Flags = 0;

    //    hr = globals->m_device->CreateUnorderedAccessView(m_buffer.Get(), &uavDesc, m_bufferUAV.GetAddressOf());
    //    assert(hr >= 0 && "Failed to create UAV\n");
    //}
}

void Buffer::CreateAndInitDefaultBuffer(void *initData, uint32_t byteSize)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    D3D12_HEAP_PROPERTIES defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    hr = globals->m_device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(m_buffer.GetAddressOf()));
    assert(hr >= 0 && "Failed to create default buffer\n");

    ComPtr<ID3D12Resource> m_uploadBuffer;
    D3D12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    hr = globals->m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_uploadBuffer.GetAddressOf()));
    assert(hr >= 0 && "Failed to create upload buffer\n");

    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    globals->BeginCommandsRecording();

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST);
    globals->m_commandList->ResourceBarrier(1, &barrier);

    UpdateSubresources<1>(globals->m_commandList.Get(),
        m_buffer.Get(),
        m_uploadBuffer.Get(),
        0,
        0,
        1,
        &subResourceData);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ);
    globals->m_commandList->ResourceBarrier(1, &barrier);

    // m_uploadBuffer must remain alive until copying is done
    // (until command list has not been executed)
    globals->EndCommandsRecording();
    globals->Submit();
    globals->FlushCommandQueue();
}

uint32_t Buffer::CalcConstantBufferByteSize(uint32_t byteSize)
{
    return (byteSize + 255) & ~255;
}

void Buffer::CreateConstantBuffer(uint32_t byteSize)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    D3D12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(CalcConstantBufferByteSize(byteSize));

    hr = globals->m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_buffer));
    assert(hr >= 0 && "Failed to create constant buffer\n");

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_buffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = CalcConstantBufferByteSize(byteSize);

    globals->m_device->CreateConstantBufferView(&cbvDesc, globals->GetCBVDescriptor(0));
}

// only for resources in D3D12_HEAP_TYPE_UPLOAD
void Buffer::Update(void *data, uint32_t byteSize)
{
    void *mappedData = nullptr;
    m_buffer->Map(0, nullptr, &mappedData);

    memcpy(mappedData, data, byteSize);

    m_buffer->Unmap(0, nullptr);
    mappedData = nullptr;
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
    case BufferUsage_InstanceBuffer:
    {
        D3D12_VERTEX_BUFFER_VIEW VBV = {};
        VBV.BufferLocation = m_buffer->GetGPUVirtualAddress();
        VBV.SizeInBytes = m_byteSize;
        VBV.StrideInBytes = m_stride;

        D3D12_VERTEX_BUFFER_VIEW vertexBuffers[] = { VBV };
        globals->m_commandList->IASetVertexBuffers(slot, 1, vertexBuffers);

        break;
    }
    case BufferUsage_IndexBuffer:
    {
        D3D12_INDEX_BUFFER_VIEW IBV = {};
        IBV.BufferLocation = m_buffer->GetGPUVirtualAddress();
        IBV.SizeInBytes = m_byteSize;
        IBV.Format = DXGI_FORMAT_R16_UINT;

        globals->m_commandList->IASetIndexBuffer(&IBV);

        break;
    }
    case BufferUsage_ConstantBuffer:
    {
        //assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, buffer will not be bound!");

        //if (stages & ShaderStage_VertexShader)
        //    globals->m_deviceContext->VSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        //if (stages & ShaderStage_HullShader)
        //    globals->m_deviceContext->HSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        //if (stages & ShaderStage_DomainShader)
        //    globals->m_deviceContext->DSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        //if (stages & ShaderStage_GeometryShader)
        //    globals->m_deviceContext->GSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        //if (stages & ShaderStage_PixelShader)
        //    globals->m_deviceContext->PSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        //if (stages & ShaderStage_ComputeShader)
        //    globals->m_deviceContext->CSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());

        break;
    }
    case BufferUsage_ReadBuffer:
    case BufferUsage_ReadStructuredBuffer:
    {
        //assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, buffer will not be bound!");

        //if (stages & ShaderStage_VertexShader)
        //    globals->m_deviceContext->VSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        //if (stages & ShaderStage_HullShader)
        //    globals->m_deviceContext->HSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        //if (stages & ShaderStage_DomainShader)
        //    globals->m_deviceContext->DSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        //if (stages & ShaderStage_GeometryShader)
        //    globals->m_deviceContext->GSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        //if (stages & ShaderStage_PixelShader)
        //    globals->m_deviceContext->PSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        //if (stages & ShaderStage_ComputeShader)
        //    globals->m_deviceContext->CSSetShaderResources(slot, 1, m_bufferSRV.GetAddressOf());

        break;
    }
    case BufferUsage_RWBuffer:
    case BufferUsage_RWStructuredBuffer:
    {
        //assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, buffer will not be bound!");

        //if (stages & ShaderStage_ComputeShader)
        //{
        //    globals->m_deviceContext->CSSetUnorderedAccessViews(
        //        slot,
        //        1,
        //        m_bufferUAV.GetAddressOf(),
        //        nullptr);
        //}
        //else
        //{
        //    globals->m_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
        //        D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
        //        nullptr,
        //        nullptr,
        //        1 + slot, // u0 in PS is used for RTV
        //        1,
        //        m_bufferUAV.GetAddressOf(),
        //        nullptr);
        //}
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

    switch (m_usage)
    {
    case BufferUsage_VertexBuffer:
    case BufferUsage_InstanceBuffer:
    {
        globals->m_commandList->IASetVertexBuffers(m_bindInfo.slot, 1, nullptr);

        break;
    }
    case BufferUsage_IndexBuffer:
    {
        globals->m_commandList->IASetIndexBuffer(nullptr);

        break;
    }
    case BufferUsage_ConstantBuffer:
    {
        //if (m_bindInfo.stages & ShaderStage_VertexShader)
        //    globals->m_deviceContext->VSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        //if (m_bindInfo.stages & ShaderStage_HullShader)
        //    globals->m_deviceContext->HSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        //if (m_bindInfo.stages & ShaderStage_DomainShader)
        //    globals->m_deviceContext->DSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        //if (m_bindInfo.stages & ShaderStage_GeometryShader)
        //    globals->m_deviceContext->GSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        //if (m_bindInfo.stages & ShaderStage_PixelShader)
        //    globals->m_deviceContext->PSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        //if (m_bindInfo.stages & ShaderStage_ComputeShader)
        //    globals->m_deviceContext->CSSetConstantBuffers(m_bindInfo.slot, 1, &nullBuffer);

        break;
    }
    case BufferUsage_ReadBuffer:
    case BufferUsage_ReadStructuredBuffer:
    {
        //ID3D11ShaderResourceView *nullSRV = nullptr;

        //if (m_bindInfo.stages & ShaderStage_VertexShader)
        //    globals->m_deviceContext->VSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        //if (m_bindInfo.stages & ShaderStage_HullShader)
        //    globals->m_deviceContext->HSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        //if (m_bindInfo.stages & ShaderStage_DomainShader)
        //    globals->m_deviceContext->DSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        //if (m_bindInfo.stages & ShaderStage_GeometryShader)
        //    globals->m_deviceContext->GSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        //if (m_bindInfo.stages & ShaderStage_PixelShader)
        //    globals->m_deviceContext->PSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        //if (m_bindInfo.stages & ShaderStage_ComputeShader)
        //    globals->m_deviceContext->CSSetShaderResources(m_bindInfo.slot, 1, &nullSRV);

        break;
    }
    case BufferUsage_RWBuffer:
    case BufferUsage_RWStructuredBuffer:
    {
        //ID3D11UnorderedAccessView *nullUAV = nullptr;

        //if (m_bindInfo.stages & ShaderStage_ComputeShader)
        //{
        //    globals->m_deviceContext->CSSetUnorderedAccessViews(
        //        m_bindInfo.slot,
        //        1,
        //        &nullUAV,
        //        nullptr);
        //}
        //else
        //{
        //    globals->m_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
        //        D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
        //        nullptr,
        //        nullptr,
        //        1 + m_bindInfo.slot, // u0 in PS is used for RTV
        //        1,
        //        &nullUAV,
        //        nullptr);
        //}
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