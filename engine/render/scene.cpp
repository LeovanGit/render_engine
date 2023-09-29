#include "scene.h"

namespace engine
{
void Scene::AddSphere(const math::Sphere &sphere)
{
    m_spheres.push_back(sphere);
}

void Scene::Render(Window &window)
{
    window.Clear();

    std::vector<uint32_t> &pixels = window.GetPixelsBuffer();
    uint32_t size = pixels.size();
    glm::u16vec2 size_2d = window.GetPixelsBufferSize();

    math::Intersection intersection;

    for (uint32_t i = 0; i != size; ++i)
    {
        uint32_t row = i / size_2d.x;
        uint32_t col = i % size_2d.x;

        // convert pixels buffer index (row; col) into Cartesian
        // with (0; 0) at the center and x - right, y - up:
        float x = col - float(size_2d.x) / 2.0f + 0.5f; // +0.5f to cast ray from the pixel's center
        float y = -(row - float(size_2d.y) / 2.0f + 0.5f); // invert to get the up direction

        // orthographic projection (parallel rays)
        math::Ray ray(glm::vec3(x, y, -10.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));

        for (const auto &sphere : m_spheres) sphere.IsIntersected(ray, intersection);

        if (intersection) pixels[i] = HEX_COLORS::WHITE;

        intersection.Reset();
    }
}
} // engine
