#include "scene.h"

namespace engine
{
void Scene::AddSphere(const math::Sphere &sphere)
{
    m_spheres.push_back(sphere);
}

void Scene::CreateDepthBuffer(uint16_t client_width, uint16_t client_height)
{
    const Direct3D *d3d = Direct3D::GetInstance();

    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depth_stencil_desc;
    depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depth_stencil_desc.Alignment = 0;
    depth_stencil_desc.Width = client_width;
    depth_stencil_desc.Height = client_height;
    depth_stencil_desc.DepthOrArraySize = 1;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
    depth_stencil_desc.SampleDesc.Count = 1;
    depth_stencil_desc.SampleDesc.Quality = 0;
    depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clear_value;
    clear_value.Format = depth_stencil_desc.Format;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0;
    
    D3D12_HEAP_PROPERTIES heap_properties;
    heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_properties.CreationNodeMask = 1;
    heap_properties.VisibleNodeMask = 1;

    HRESULT result = d3d->m_device->CreateCommittedResource(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &depth_stencil_desc,
        D3D12_RESOURCE_STATE_COMMON,
        &clear_value,
        IID_PPV_ARGS(m_depth_stencil_buffer.GetAddressOf()));

    // Create descriptor to mip level 0:
    d3d->m_device->CreateDepthStencilView(
        m_depth_stencil_buffer.Get(),
        nullptr,
        d3d->GetDepthStencilView());

    D3D12_RESOURCE_TRANSITION_BARRIER transition_barrier;
    transition_barrier.pResource = m_depth_stencil_buffer.Get();
    transition_barrier.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    transition_barrier.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    transition_barrier.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;

    // Transition the resource from its initial state to be used as a depth buffer
    D3D12_RESOURCE_BARRIER resource_barrier;
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    resource_barrier.Transition = transition_barrier;

    d3d->m_command_list->ResourceBarrier(1, &resource_barrier);
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
