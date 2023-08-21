#include "intersection.h"

namespace math
{
Intersection::Intersection()
{
    Reset();
}

void Intersection::Reset()
{
    t = std::numeric_limits<float>::infinity();
}

Intersection::operator bool() const
{
    return t < std::numeric_limits<float>::infinity();
}

} // namespace math