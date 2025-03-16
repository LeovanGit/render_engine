#pragma once

#include <iostream>

#include "utils/d3dcommon.h"
#include "globals.h"

namespace engine
{
enum TextureUsage
{
    TextureUsage_SRV = (1 << 0),
    TextureUsage_UAV = (1 << 1)
};

class Texture
{
public:
    Texture(const std::wstring &pathToFile, TextureUsage usage);

    ~Texture() = default;

    std::wstring m_pathToFile;

    ComPtr<ID3D12Resource> m_texture;
    uint32_t m_indexInHeap;

private:
    void CreateTexture(const std::wstring &pathToFile);
    void CreateShaderResourceView();
};
} // namespace engine
