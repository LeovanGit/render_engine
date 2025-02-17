#include "sky.h"

namespace engine
{
Sky::Sky(const std::wstring &pathToCubemap)
{
    ShaderManager *sm = ShaderManager::GetInstance();
    TextureManager *tm = TextureManager::GetInstance();

    m_shader = sm->GetOrCreateShader(
        ShaderStage_VertexShader |
        ShaderStage_PixelShader,
        L"../assets/shaders/skybox.hlsl");

    m_cubemap = tm->GetOrCreateTexture(pathToCubemap);
}

Sky::~Sky()
{
    // Just break references to them, they will be destructed in Engine::Deinit():
    m_cubemap = nullptr;
    m_shader = nullptr;
}

void Sky::Render()
{
    Globals *globals = Globals::GetInstance();

    m_shader->Bind();
    m_cubemap->Bind(0, ShaderStage_PixelShader);
    
    globals->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    globals->m_deviceContext->Draw(3, 0);

    // Unbind after draw:
    m_cubemap->Unbind();
    m_shader->Unbind();
}
} // namespace engine
