#pragma once

#include "renderer/model_manager.h"
#include "graphics_context/texture_manager.h"
#include "graphics_context/buffer.h"
#include "graphics_context/shader_manager.h"
#include "graphics_context/pipeline_manager.h"

#include <DirectXMath.h>

namespace engine
{
class OpaqueInstances
{
public:
    // Helper structures
    struct InstanceData
    {
        DirectX::XMFLOAT4X4 m_modelMatrix;
    };

    struct Material
    {
        std::shared_ptr<Texture> m_texture;
    };

    // Data-Oriented Design
    // group all instances with the same material:
    struct PerMaterial
    {
        Material m_material;
        std::vector<InstanceData> m_instances;
    };

    // group all meshes with the same vertex buffer:
    struct PerMesh
    {
        std::shared_ptr<Mesh> m_mesh;
        std::vector<PerMaterial> m_perMaterial;
    };

    std::vector<PerMesh> m_meshes;
    std::shared_ptr<Buffer> m_instanceBuffer; // all instances in one buffer (we will use offsets)
    std::shared_ptr<Pipeline> m_PSO; // shader + states
    
    OpaqueInstances();
    ~OpaqueInstances();

    void AddInstance(
        const std::wstring &pathToTexture,
        std::shared_ptr<Mesh> mesh,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 rotation);

    void Render();
};
} // namespace engine
