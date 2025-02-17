#pragma once

#include "graphics_context/shader_manager.h"
#include "renderer/model_manager.h"

namespace engine
{
class Terrain
{
public:
    Terrain(const std::wstring &pathToHeightMap,
        uint32_t rows,
        uint32_t cols,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);
    
    ~Terrain();

    void Render();

    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Mesh> m_mesh;
};
} // namespace engine