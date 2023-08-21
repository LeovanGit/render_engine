#pragma once

#include <limits>

namespace math
{
class Intersection
{
public:
    Intersection();

    void Reset();

    operator bool() const;

    float t;
};
} // namespace math