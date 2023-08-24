#pragma once

#include "glm\glm.hpp"

namespace math
{
class Ray
{
public:
    Ray(const glm::vec3 &origin,
        const glm::vec3 &direction);

    glm::vec3 m_origin;
    glm::vec3 m_direction;
};
} // namespace math
