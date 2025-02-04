#pragma once

#include "window/window.h"
#include "camera.h"
#include "shader_manager.h"
#include "model_manager.h"

namespace engine
{
class Renderer
{
public:
    Renderer(Window *window);
    ~Renderer() = default;

    void Update();
    void Render(bool debugMode);
    void RenderOpaque();
    void RenderDebug();

    void UnbindAll();
    void Destroy();

    struct PerViewConstantBuffer
    {
        DirectX::XMFLOAT4X4 viewProjMatrix;
    };

    struct PerMeshConstantBuffer
    {
        DirectX::XMFLOAT4X4 modelMatrix;
    };

    void CreateConstantBuffers();
    void UpdatePerViewConstantBuffer(PerViewConstantBuffer &data);
    void UpdatePerMeshConstantBuffer(PerMeshConstantBuffer &data);
    void BindPerViewConstantBuffer();
    void BindPerMeshConstantBuffer();

    Window *m_window;

    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Shader> m_currentShader;
    std::shared_ptr<Shader> m_debugShader;
    std::vector< std::shared_ptr<Mesh>> m_meshes;

    std::shared_ptr<Buffer> m_perViewConstantBuffer;
    std::shared_ptr<Buffer> m_perMeshConstantBuffer;
};
} // namespace engine
