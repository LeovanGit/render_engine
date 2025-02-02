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
    void Render();

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
    void BindConstantBuffers();
    void UpdatePerViewConstantBuffer(PerViewConstantBuffer &data);
    void UpdatePerMeshConstantBuffer(PerMeshConstantBuffer &data);

    Window *m_window;

    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Shader> m_currentShader;
    std::shared_ptr<Mesh> m_mesh;

    std::shared_ptr<Buffer> m_perViewConstantBuffer;
    std::shared_ptr<Buffer> m_perMeshConstantBuffer;
};
} // namespace engine
