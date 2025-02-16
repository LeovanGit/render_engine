#pragma once

#include <iostream>

#include "utils/d3d11common.h"
#include "render/globals.h"

namespace engine
{
class Texture
{
public:
    Texture(const std::wstring &pathToFile);
    Texture(bool createEmptyUAV);

    ~Texture() = default;

public:
    void Bind(uint8_t slot);

    std::wstring m_pathToFile;

    ComPtr<ID3D11ShaderResourceView> m_textureSRV;
    ComPtr<ID3D11UnorderedAccessView> m_textureUAV;
};
} // namespace engine
