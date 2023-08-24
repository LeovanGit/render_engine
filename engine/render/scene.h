#pragma once

#include <vector>

#include "../windows/window.h"
#include "../source/math/sphere.h"
#include "../source/math/ray.h"
#include "../source/math/intersection.h"

class Scene
{
public:
    Scene() = default;

    void AddSphere(const math::Sphere &sphere);

    void Render(Window &window);

    std::vector<math::Sphere> m_spheres;
};