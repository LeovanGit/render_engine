#pragma once

#include <vector>

#include "../windows/window.h"
#include "../source/math/sphere.h"
#include "../source/math/ray.h"
#include "../source/math/intersection.h"

namespace engine
{
class Scene
{
public:
    Scene() = default;

    void AddSphere(const math::Sphere &sphere);

    void CreateDepthBuffer(uint16_t width, uint16_t height);

    void Render(Window &window);

    std::vector<math::Sphere> m_spheres;

    wrl::ComPtr<ID3D12Resource> m_depth_stencil_buffer;
};
} // namespace engine
