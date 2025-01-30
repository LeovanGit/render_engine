#pragma once

#include <DirectXMath.h>

namespace engine
{
class Camera
{
public:
    Camera(DirectX::XMVECTOR position,
        DirectX::XMVECTOR forwardDirection,
        DirectX::XMVECTOR upDirection,
        float aspectRatio);
    ~Camera() = default;

    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projMatrix;

    DirectX::XMMATRIX m_viewProjMatrix;
};
} // namespace engine