#include "Texture.h"

#include <cassert>

#include <DDSTextureLoader11.h>

namespace engine
{
Texture::Texture(const std::wstring &pathToFile)
    : m_pathToFile(pathToFile)
    , m_texture(nullptr)
{
    Globals *globals = Globals::GetInstance();

    HRESULT hr = DirectX::CreateDDSTextureFromFile(
        globals->m_device.Get(),
        m_pathToFile.c_str(),
        nullptr,
        m_texture.GetAddressOf());
    assert(hr >= 0 && "Failed to create texture\n");
}

void Texture::Bind(uint8_t slot)
{
    Globals *globals = Globals::GetInstance();
    
    globals->m_deviceContext->PSSetShaderResources(
        slot,
        1,
        m_texture.GetAddressOf());

    globals->m_deviceContext->DSSetShaderResources(
        slot,
        1,
        m_texture.GetAddressOf());
}
} // namespace engine
