#include "Texture.h"

#include <cassert>

#include <DDSTextureLoader11.h>

namespace engine
{
Texture::Texture(const std::wstring &pathToFile)
    : m_pathToFile(pathToFile)
    , m_textureSRV(nullptr)
    , m_textureUAV(nullptr)
{
    Globals *globals = Globals::GetInstance();

    HRESULT hr = DirectX::CreateDDSTextureFromFile(
        globals->m_device.Get(),
        m_pathToFile.c_str(),
        nullptr,
        m_textureSRV.GetAddressOf());
    assert(hr >= 0 && "Failed to create texture\n");
}

Texture::Texture(bool createEmptyUAV)
{
    Globals *globals = Globals::GetInstance();

    if (createEmptyUAV)
    {
        HRESULT hr;

        ComPtr<ID3D11Texture2D> texture;

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = 1280;
        textureDesc.Height = 720;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;

        hr = globals->m_device->CreateTexture2D(
            &textureDesc,
            nullptr,
            texture.GetAddressOf());
        assert(hr >= 0 && "Failed to create texture\n");

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

        globals->m_device->CreateUnorderedAccessView(
            texture.Get(),
            &uavDesc,
            m_textureUAV.GetAddressOf());
        assert(hr >= 0 && "Failed to create UAV\n");
    }
}

void Texture::Bind(uint8_t slot)
{
    Globals *globals = Globals::GetInstance();
    
    if (m_textureSRV)
    {
        globals->m_deviceContext->PSSetShaderResources(
            slot,
            1,
            m_textureSRV.GetAddressOf());

        globals->m_deviceContext->DSSetShaderResources(
            slot,
            1,
            m_textureSRV.GetAddressOf());
    }
    else
    {
        globals->m_deviceContext->CSSetUnorderedAccessViews(
            slot,
            1,
            m_textureUAV.GetAddressOf(),
            nullptr);
    }
}
} // namespace engine
