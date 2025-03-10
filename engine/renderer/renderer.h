#pragma once

#include "window/window.h"
#include "camera.h"
#include "opaque_instances.h"
#include "terrain.h"
#include "sky.h"
#include "postprocess.h"
#include "quad_sphere.h"
#include "graphics_context/pipeline_manager.h"

namespace engine
{
struct PerViewData
{
    DirectX::XMFLOAT4X4 viewProjMatrix;
    DirectX::XMFLOAT3 cameraPostionWS;
    float offset0;
    // camera frustum near plane's corners in WS:
    DirectX::XMFLOAT4 nearPlaneCornersWS[3];
};

class Renderer
{
public:
    Renderer(std::shared_ptr<Window> window);
    ~Renderer() = default;

    void CreateConstantBuffers();
    void UpdatePerViewConstantBuffer();

    void Render(bool debugMode);

    void Destroy();

    std::shared_ptr<Window> m_window;
    std::shared_ptr<Camera> m_camera;

    std::shared_ptr<Buffer> m_perViewConstantBuffer;

    std::shared_ptr<OpaqueInstances> m_opaqueInstances;
    //std::shared_ptr<Terrain> m_terrain;
    //std::shared_ptr<Sky> m_skybox;

    //std::shared_ptr<PostProcess> m_postprocess;

    //std::shared_ptr<QuadSphere> m_quadSphere;
};
} // namespace engine
