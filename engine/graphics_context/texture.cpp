#include "Texture.h"

#include <cassert>

#include <DDSTextureLoader12.h>

#include "buffer.h"

namespace engine
{
Texture::Texture(const std::wstring &pathToFile, TextureUsage usage)
    : m_pathToFile(pathToFile)
    , m_texture(nullptr)
    , m_indexInHeap(-1)
{
    CreateTexture(pathToFile);
    CreateShaderResourceView();
}

void Texture::CreateTexture(const std::wstring &pathToFile)
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    std::unique_ptr<uint8_t[]> ddsData;
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;

    hr = DirectX::LoadDDSTextureFromFile(
        globals->m_device.Get(),
        pathToFile.c_str(),
        m_texture.GetAddressOf(),
        ddsData,
        subresources);
    assert(hr >= 0 && "Failed to load .dds texture from file\n");

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(
        m_texture.Get(),
        0,
        subresources.size());

    std::shared_ptr<Buffer> uploadBuffer = std::make_shared<Buffer>(
        BufferUsage_UploadBuffer,
        nullptr,
        uploadBufferSize);

    globals->BeginCommandsRecording();

    UpdateSubresources(
        globals->m_commandList.Get(),
        m_texture.Get(),
        uploadBuffer->m_buffer.Get(),
        0,
        0,
        subresources.size(),
        subresources.data());

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_texture.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    globals->m_commandList->ResourceBarrier(1, &barrier);

    // uploadBuffer must remain alive until copying is done
    // (until command list has not been executed):
    globals->EndCommandsRecording();
    globals->Submit();
    globals->FlushCommandQueue();
}

void Texture::CreateShaderResourceView()
{
    Globals *globals = Globals::GetInstance();

    D3D12_RESOURCE_DESC textureDesc = m_texture->GetDesc();
    
    D3D12_SRV_DIMENSION viewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    // TODO: this is a little hacky
    // Texture array also can be with D3D12_RESOURCE_DIMENSION_TEXTURE2D and DepthOrArraySize == 6
    // Use something like DirectX::TexMetadata from LoadFromDDSFile()
    if (textureDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D &&
        textureDesc.DepthOrArraySize == 6)
    {
        viewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = textureDesc.Format;
    SRVDesc.ViewDimension = viewDimension;
    SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = textureDesc.MipLevels;
    SRVDesc.Texture2D.PlaneSlice = 0;
    SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    m_indexInHeap = globals->GetNextFreeCBV_SRV_UAVDescriptorIndex();

    globals->m_device->CreateShaderResourceView(
        m_texture.Get(),
        &SRVDesc,
        globals->GetCBV_SRV_UAVDescriptor(m_indexInHeap));
}
} // namespace engine
