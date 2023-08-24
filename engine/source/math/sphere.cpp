#include "sphere.h"

namespace math
{
Sphere::Sphere(const glm::vec3 &origin, float radius) : m_origin(origin), m_radius(radius) {}

bool Sphere::IsIntersected(const Ray &ray, Intersection &outIntersection) const
{
    // vector from ray origin to sphere origin:
    glm::vec3 L = m_origin - ray.m_origin;

    // projection of L on ray.direction:
    float prL = glm::dot(L, ray.m_direction);

    // square distance from sphere origin to ray:
    float h2 = glm::length(L) * glm::length(L) - prL * prL;
    if (h2 > m_radius * m_radius) return false; // no intersection

    float tNear = prL - sqrtf(m_radius * m_radius - h2);
    //float tFar = prL + sqrtf(radius * radius - h2);

    // return true only if this is nearest intersection:
    if (tNear >= outIntersection.t) return false;

    outIntersection.t = tNear;
    return true;
}
} // namespace math