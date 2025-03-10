#pragma once

#include <iostream>
#include <unordered_map>

#include "texture.h"

namespace engine
{
class TextureManager : public NonCopyable
{
public:
    static void Create();
    static TextureManager *GetInstance();
    static void Destroy();

    std::shared_ptr<Texture> GetOrCreateTexture(
        const std::wstring &pathToFile,
        TextureUsage usage);

    std::unordered_map<std::wstring, std::shared_ptr<Texture>> m_textures;

private:
    TextureManager() = default;
    ~TextureManager() = default;

    static TextureManager *s_instance;
};
} // namespace engine
