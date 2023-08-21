#pragma once

#include "glm/glm.hpp"

#include "ray.h"
#include "intersection.h"

namespace math
{
class Sphere
{
public:
    Sphere(const glm::vec3 &origin, float radius);

    bool IsIntersected(const Ray &ray, Intersection &outIntersection) const;

    glm::vec3 m_origin;
    float m_radius;
};
} // namespace math
