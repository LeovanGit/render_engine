#include "sky.h"

namespace engine
{
Sky::Sky(const std::wstring &pathToCubemap)
{
    ShaderManager *sm = ShaderManager::GetInstance();
    TextureManager *tm = TextureManager::GetInstance();

    m_shader = sm->GetOrCreateShader(
        engine::ShaderStage_VertexShader |
        engine::ShaderStage_PixelShader,
        L"../assets/shaders/skybox.hlsl");

    m_cubemap = tm->GetOrCreateTexture(pathToCubemap);
}

void Sky::Render()
{
    // TODO
}
} // namespace engine
