#pragma once

#include <DirectXMath.h>

namespace engine
{
using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;

using DirectX::XMVectorZero;
using DirectX::XMStoreFloat3;
using DirectX::XMLoadFloat3;
using DirectX::XMVector3Cross;
using DirectX::XMVectorAdd;
using DirectX::XMVector4Transform;
using DirectX::XMStoreFloat4;
using DirectX::XMLoadFloat4;
using DirectX::XMVectorSplatW;
using DirectX::XMVectorDivide;

using DirectX::XMStoreFloat4x4;
using DirectX::XMLoadFloat4x4;
using DirectX::XMMatrixPerspectiveFovLH;
using DirectX::XMMatrixLookToLH;
using DirectX::XMMatrixMultiply;
using DirectX::XMMatrixInverse;

using DirectX::XMConvertToRadians;
using DirectX::XMConvertToDegrees;
using DirectX::XMQuaternionRotationAxis;
using DirectX::XMQuaternionMultiply;
using DirectX::XMVector3Rotate;

class Camera
{
public:
    Camera(uint16_t width,
        uint16_t height,
        float nearPlane,
        float farPlane,
        float fovY);

    ~Camera() = default;

    void ChangeSettings(
        uint16_t width,
        uint16_t height,
        float nearPlane,
        float farPlane,
        float fovY);

    void UpdateMatrices();

    XMMATRIX GetViewProjMatrix();

    void SetPosition(float x, float y, float z);
    void AddPosition(float x, float y, float z);

    // Rotate camera around it's basis vectors
    // in 3D space: x-axis, y-axis, z-axis, all in degrees
    void AddRotation(float pitch, float yaw, float roll);

    XMVECTOR GetPosition() const;

    XMVECTOR GetRight() const;
    XMVECTOR GetUp() const;
    XMVECTOR GetForward() const;

    float GetWidth() const { return m_width; }
    float GetHeight() const { return m_height; }
    float GetNearPlane() const { return m_nearPlane; };
    float GetFarPlane() const { return m_farPlane; };
    float GetFOVY() const { return m_fovY; };
    float GetFOVX() const { return m_fovX; }

    XMVECTOR Reproject(float x, float y) const;

private:
    XMFLOAT3 m_position;

    // basis (should be unit-size):
    XMFLOAT3 m_forward;
    XMFLOAT3 m_up;
    XMFLOAT3 m_right;

    // size of window (near plane):
    uint16_t m_width;
    uint16_t m_height;
    float m_aspectRatio; // width / height

    float m_nearPlane;
    float m_farPlane;

    float m_fovY; // in degrees
    float m_fovX;
    
    bool m_dirtyFlag; // true - need to update matrices

    XMFLOAT4X4 m_viewMatrix;
    XMFLOAT4X4 m_projMatrix;

    XMFLOAT4X4 m_viewProjMatrix;
    XMFLOAT4X4 m_invViewProjMatrix;
};
} // namespace engine
