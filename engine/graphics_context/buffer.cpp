#include "buffer.h"

#include <cassert>

namespace engine
{
Buffer::Buffer(BufferUsage usage, void *data, uint32_t byteSize, uint32_t stride)
    : m_buffer(nullptr)
    , m_usage(usage)
    , m_byteSize(byteSize)
    , m_stride(stride)
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
    case BufferUsage_UploadBuffer:
    {
        CreateUploadBuffer(byteSize);

        break;
    }
    default:
        assert(false && "Unknown buffer usage\n");
    }
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

    ComPtr<ID3D12Resource> uploadBuffer;
    D3D12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    hr = globals->m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf()));
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
        uploadBuffer.Get(),
        0,
        0,
        1,
        &subResourceData);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ);
    globals->m_commandList->ResourceBarrier(1, &barrier);

    // uploadBuffer must remain alive until copying is done
    // (until command list has not been executed):
    globals->EndCommandsRecording();
    globals->Submit();
    globals->FlushCommandQueue();
}

void Buffer::CreateConstantBuffer(uint32_t byteSize)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    uint32_t alignedByteSize = AlignUpToMultipleOf256<uint32_t>(byteSize);

    D3D12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedByteSize);

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
    cbvDesc.SizeInBytes = alignedByteSize;

    static uint32_t indexInCBVHeap = 0;
    globals->m_device->CreateConstantBufferView(&cbvDesc, globals->GetCBVDescriptor(indexInCBVHeap));
    ++indexInCBVHeap;
}

void Buffer::CreateUploadBuffer(uint32_t byteSize)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    D3D12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    hr = globals->m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_buffer));
    assert(hr >= 0 && "Failed to create upload buffer\n");
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

void Buffer::BindVertexBuffer(uint32_t slot, std::shared_ptr<Buffer> buffer)
{
    assert((buffer->m_usage == BufferUsage_VertexBuffer ||
        buffer->m_usage == BufferUsage_InstanceBuffer) && "Must be Vertex/Instance Buffer");

    Globals *globals = Globals::GetInstance();

    D3D12_VERTEX_BUFFER_VIEW VBV = {};
    VBV.BufferLocation = buffer->m_buffer->GetGPUVirtualAddress();
    VBV.SizeInBytes = buffer->m_byteSize;
    VBV.StrideInBytes = buffer->m_stride;

    globals->m_commandList->IASetVertexBuffers(slot, 1, &VBV);
}

void Buffer::BindIndexBuffer(std::shared_ptr<Buffer> buffer)
{
    assert(buffer->m_usage == BufferUsage_IndexBuffer && "Must be Index Buffer");

    Globals *globals = Globals::GetInstance();

    D3D12_INDEX_BUFFER_VIEW IBV = {};
    IBV.BufferLocation = buffer->m_buffer->GetGPUVirtualAddress();
    IBV.SizeInBytes = buffer->m_byteSize;
    IBV.Format = DXGI_FORMAT_R16_UINT;

    globals->m_commandList->IASetIndexBuffer(&IBV);
}
} // namespace engine