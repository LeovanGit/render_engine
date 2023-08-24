#include "ray.h"

namespace math
{
Ray::Ray(const glm::vec3 &origin,
         const glm::vec3 &direction) :
         m_origin(origin),
         m_direction(glm::normalize(direction)) {}
} // namespace math