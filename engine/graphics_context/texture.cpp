#include "Texture.h"

#include <cassert>

//#include <DDSTextureLoader11.h>

namespace engine
{
// create empty texture
/*Texture::Texture(uint32_t width, uint32_t height, TextureUsage usage)
    : m_pathToFile(L"")
    , m_width(width)
    , m_height(height)
    , m_textureSRV(nullptr)
    , m_textureUAV(nullptr)
    , m_bindInfo({ false, 0, TextureUsage_SRV, ShaderStage_None })
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    ComPtr<ID3D11Texture2D> texture;

    D3D11_USAGE usageFlag = D3D11_USAGE_IMMUTABLE;
    if (usage & TextureUsage_UAV) usageFlag = D3D11_USAGE_DEFAULT;

    UINT bindFlags = 0;
    if (usage & TextureUsage_SRV) bindFlags |= D3D11_BIND_SHADER_RESOURCE;
    if (usage & TextureUsage_UAV) bindFlags |= D3D11_BIND_UNORDERED_ACCESS;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = usageFlag;
    textureDesc.BindFlags = bindFlags;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    hr = globals->m_device->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf());
    assert(hr >= 0 && "Failed to create texture\n");

    if (usage & TextureUsage_SRV)
    {
        // set D3D11_SHADER_RESOURCE_VIEW_DESC to nullptr to create SRV that
        // accesses the entire resource (using format from textureDesc):
        hr = globals->m_device->CreateShaderResourceView(texture.Get(), nullptr, m_textureSRV.GetAddressOf());
        assert(hr >= 0 && "Failed to create SRV\n");
    }

    if (usage & TextureUsage_UAV)
    {
        // same for UAV:
        hr = globals->m_device->CreateUnorderedAccessView(texture.Get(), nullptr, m_textureUAV.GetAddressOf());
        assert(hr >= 0 && "Failed to create UAV\n");
    }
}

// create texture from file
Texture::Texture(const std::wstring &pathToFile, TextureUsage usage)
    : m_pathToFile(pathToFile)
    , m_width(0)
    , m_height(0)
    , m_textureSRV(nullptr)
    , m_textureUAV(nullptr)
    , m_bindInfo({ false, 0, TextureUsage_SRV, ShaderStage_None })
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    ComPtr<ID3D11Resource> texture;
    
    D3D11_USAGE usageFlag = D3D11_USAGE_IMMUTABLE;
    if (usage & TextureUsage_UAV) usageFlag = D3D11_USAGE_DEFAULT;

    UINT bindFlags = 0;
    if (usage & TextureUsage_SRV) bindFlags |= D3D11_BIND_SHADER_RESOURCE;
    if (usage & TextureUsage_UAV) bindFlags |= D3D11_BIND_UNORDERED_ACCESS;

    hr = DirectX::CreateDDSTextureFromFileEx(
        globals->m_device.Get(),
        m_pathToFile.c_str(),
        0,
        usageFlag,
        bindFlags,
        0,
        0,
        false,
        texture.GetAddressOf(),
        usage & TextureUsage_SRV ? m_textureSRV.GetAddressOf() : nullptr);
    assert(hr >= 0 && "Failed to create texture\n");

    ComPtr<ID3D11Texture2D> texture2D;
    hr = texture->QueryInterface(IID_PPV_ARGS(texture2D.GetAddressOf()));
    assert(hr >= 0 && "Failed to query ID3D11Texture2D from ID3D11Resource\n");

    D3D11_TEXTURE2D_DESC textureDesc = {};
    texture2D->GetDesc(&textureDesc);
    m_width = textureDesc.Width;
    m_height = textureDesc.Height;

    if (usage & TextureUsage_UAV)
    {
        hr = globals->m_device->CreateUnorderedAccessView(texture.Get(), nullptr, m_textureUAV.GetAddressOf());
        assert(hr >= 0 && "Failed to create UAV\n");
    }
}

void Texture::Bind(uint32_t slot, TextureUsage usage, ShaderStage stages)
{
    Globals *globals = Globals::GetInstance();
    
    assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, texture will not be bound!");
    assert(stages != (TextureUsage_SRV | TextureUsage_UAV) && "can't bind texture as SRV and UAV at the same time!");

    m_bindInfo.isBound = true;
    m_bindInfo.slot = slot;
    m_bindInfo.usage = usage;
    m_bindInfo.stages = stages;

    if (usage & TextureUsage_SRV)
    {
        if (stages & ShaderStage_VertexShader)
        {
            globals->m_deviceContext->VSSetShaderResources(
                slot,
                1,
                m_textureSRV.GetAddressOf());
        }

        if (stages & ShaderStage_HullShader)
        {
            globals->m_deviceContext->HSSetShaderResources(
                slot,
                1,
                m_textureSRV.GetAddressOf());
        }

        if (stages & ShaderStage_DomainShader)
        {
            globals->m_deviceContext->DSSetShaderResources(
                slot,
                1,
                m_textureSRV.GetAddressOf());
        }

        if (stages & ShaderStage_GeometryShader)
        {
            globals->m_deviceContext->GSSetShaderResources(
                slot,
                1,
                m_textureSRV.GetAddressOf());
        }

        if (stages & ShaderStage_PixelShader)
        {
            globals->m_deviceContext->PSSetShaderResources(
                slot,
                1,
                m_textureSRV.GetAddressOf());
        }

        if (stages & ShaderStage_ComputeShader)
        {
            globals->m_deviceContext->CSSetShaderResources(
                slot,
                1,
                m_textureSRV.GetAddressOf());
        }
    }

    // bind UAV to OM (entire graphics pipeline, visible from all shader stages):
    if (usage & TextureUsage_UAV)
    {
        if (m_bindInfo.stages & ShaderStage_ComputeShader)
        {
            globals->m_deviceContext->CSSetUnorderedAccessViews(
                slot,
                1,
                m_textureUAV.GetAddressOf(),
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
                m_textureUAV.GetAddressOf(),
                nullptr);
        }
    }
}

void Texture::Unbind()
{
    Globals *globals = Globals::GetInstance();

    assert(m_bindInfo.isBound && "texture is not bound");

    if (m_bindInfo.usage & TextureUsage_SRV)
    {
        ID3D11ShaderResourceView *nullSRV = nullptr;

        if (m_bindInfo.stages & ShaderStage_VertexShader)
        {
            globals->m_deviceContext->VSSetShaderResources(
                m_bindInfo.slot,
                1,
                &nullSRV);
        }

        if (m_bindInfo.stages & ShaderStage_HullShader)
        {
            globals->m_deviceContext->HSSetShaderResources(
                m_bindInfo.slot,
                1,
                &nullSRV);
        }

        if (m_bindInfo.stages & ShaderStage_DomainShader)
        {
            globals->m_deviceContext->DSSetShaderResources(
                m_bindInfo.slot,
                1,
                &nullSRV);
        }

        if (m_bindInfo.stages & ShaderStage_GeometryShader)
        {
            globals->m_deviceContext->GSSetShaderResources(
                m_bindInfo.slot,
                1,
                &nullSRV);
        }

        if (m_bindInfo.stages & ShaderStage_PixelShader)
        {
            globals->m_deviceContext->PSSetShaderResources(
                m_bindInfo.slot,
                1,
                &nullSRV);
        }

        if (m_bindInfo.stages & ShaderStage_ComputeShader)
        {
            globals->m_deviceContext->CSSetShaderResources(
                m_bindInfo.slot,
                1,
                &nullSRV);
        }
    }

    if (m_bindInfo.usage & TextureUsage_UAV)
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
    }

    m_bindInfo.isBound = false;
    m_bindInfo.slot = 0;
    m_bindInfo.stages = ShaderStage::ShaderStage_None;
}*/
} // namespace engine
