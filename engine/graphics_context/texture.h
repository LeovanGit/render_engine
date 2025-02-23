#pragma once

#include <iostream>

#include "utils/d3dcommon.h"
#include "globals.h"

namespace engine
{
/*enum TextureUsage
{
    TextureUsage_SRV = (1 << 0),
    TextureUsage_UAV = (1 << 1)
};

class Texture
{
public:
    Texture(uint32_t width, uint32_t height, TextureUsage usage);
    Texture(const std::wstring &pathToFile, TextureUsage usage);

    ~Texture() = default;

public:
    void Bind(uint32_t slot, TextureUsage usage, ShaderStage stages);

    void Unbind();

    std::wstring m_pathToFile;

    uint32_t m_width;
    uint32_t m_height;

    ComPtr<ID3D11ShaderResourceView> m_textureSRV;
    ComPtr<ID3D11UnorderedAccessView> m_textureUAV;

    struct BindInfo
    {
        bool isBound;
        uint32_t slot;
        TextureUsage usage;
        ShaderStage stages;
    } m_bindInfo;
};*/
} // namespace engine
