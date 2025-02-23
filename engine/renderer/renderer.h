#pragma once

#include "window/window.h"
#include "camera.h"
#include "opaque_instances.h"
#include "terrain.h"
#include "sky.h"
#include "postprocess.h"
#include "quad_sphere.h"

namespace engine
{
class Renderer
{
public:
    Renderer(std::shared_ptr<Window> window);
    ~Renderer() = default;

    void Render(bool debugMode);

    void Destroy();

    std::shared_ptr<Window> m_window;
    std::shared_ptr<Camera> m_camera;

    //std::shared_ptr<OpaqueInstances> m_opaqueInstances;
    //std::shared_ptr<Terrain> m_terrain;
    //std::shared_ptr<Sky> m_skybox;

    //std::shared_ptr<PostProcess> m_postprocess;

    //std::shared_ptr<QuadSphere> m_quadSphere;
};
} // namespace engine
