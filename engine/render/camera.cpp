#include "camera.h"

namespace engine
{
Camera::Camera(DirectX::XMVECTOR position,
    DirectX::XMVECTOR forwardDirection,
    DirectX::XMVECTOR upDirection,
    float aspectRatio)
{
    m_viewMatrix = DirectX::XMMatrixLookToLH(position, forwardDirection, upDirection);

    m_projMatrix = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(90.0f),
        aspectRatio,
        0.1f,
        1000.0f);

    m_viewProjMatrix = DirectX::XMMatrixMultiply(m_viewMatrix, m_projMatrix);
}
} // namespace engine