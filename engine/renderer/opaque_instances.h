#pragma once

#include "graphics_context/shader_manager.h"
#include "graphics_context/pipeline_manager.h"
#include "renderer/model_manager.h"

namespace engine
{
class OpaqueInstances
{
public:
    OpaqueInstances();
    
    ~OpaqueInstances();

    void AddInstance(std::shared_ptr<Mesh> mesh);

    void Render();
    //void RenderDebug();

    std::shared_ptr<Pipeline> m_PSO;
    //std::shared_ptr<Shader> m_debugShader;

    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Buffer> m_instanceBuffer;
};
} // namespace engine
