#pragma once

#include <iostream>

#include "utils/d3d11common.h"
#include "globals.h"

namespace engine
{
class Texture
{
public:
    Texture(const std::wstring &pathToFile);

    ~Texture() = default;

public:
    void Bind(uint32_t slot, ShaderStage stages = ShaderStage::ShaderStage_None);

    void Unbind();

    std::wstring m_pathToFile;

    ComPtr<ID3D11ShaderResourceView> m_textureSRV;

    struct BindInfo
    {
        bool isBound;
        uint32_t slot;
        ShaderStage stages;
    } m_bindInfo;
};
} // namespace engine
