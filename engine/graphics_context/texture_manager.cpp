#include "texture_manager.h"

#include <cassert>

namespace engine
{
TextureManager *TextureManager::s_instance = nullptr;

void TextureManager::Create()
{
    assert(s_instance == nullptr && "TextureManager instance already created\n");
    s_instance = new TextureManager;
}

TextureManager *TextureManager::GetInstance()
{
    assert(s_instance && "TextureManager instance is not created\n");
    return s_instance;
}

void TextureManager::Destroy()
{
    assert(s_instance && "TextureManager instance is not created\n");
    delete s_instance;
    s_instance = nullptr;
}

std::shared_ptr<Texture> TextureManager::GetOrCreateTexture(
    const std::wstring &pathToFile,
    TextureUsage usage)
{
    auto found = m_textures.find(pathToFile);
    if (found != m_textures.end())
        return found->second;

    return m_textures.try_emplace(
        pathToFile,
        std::make_shared<Texture>(pathToFile, usage)).first->second;
}
} // namespace engine
