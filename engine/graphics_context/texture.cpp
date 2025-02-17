#include "Texture.h"

#include <cassert>

#include <DDSTextureLoader11.h>

namespace engine
{
Texture::Texture(const std::wstring &pathToFile)
    : m_pathToFile(pathToFile)
    , m_textureSRV(nullptr)
{
    Globals *globals = Globals::GetInstance();

    HRESULT hr = DirectX::CreateDDSTextureFromFile(
        globals->m_device.Get(),
        m_pathToFile.c_str(),
        nullptr,
        m_textureSRV.GetAddressOf());
    assert(hr >= 0 && "Failed to create texture\n");
}

void Texture::Bind(uint32_t slot, ShaderStage stages)
{
    Globals *globals = Globals::GetInstance();
    
    assert(stages != ShaderStage_None && "stages is equal to ShaderStage_None, texture will not be bound!");

    m_bindInfo.isBound = true;
    m_bindInfo.slot = slot;
    m_bindInfo.stages = stages;

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
}

void Texture::Unbind()
{
    Globals *globals = Globals::GetInstance();

    assert(m_bindInfo.isBound && "texture is not bound");

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

    m_bindInfo.isBound = false;
    m_bindInfo.slot = 0;
    m_bindInfo.stages = ShaderStage::ShaderStage_None;
}
} // namespace engine
