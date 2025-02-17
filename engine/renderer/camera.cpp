#include "camera.h"

namespace engine
{
Camera::Camera(uint16_t width,
    uint16_t height,
    float nearPlane,
    float farPlane,
    float fovY)
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    
    // left-handed:
    m_forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
    m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
    m_right = XMFLOAT3(1.0f, 0.0f, 0.0f);

    ChangeSettings(width, height, nearPlane, farPlane, fovY);
}

void Camera::ChangeSettings(
    uint16_t width,
    uint16_t height,
    float nearPlane,
    float farPlane,
    float fovY)
{
    m_width = width;
    m_height = height;
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    m_nearPlane = nearPlane;
    m_farPlane = farPlane;

    m_fovY = fovY;

    m_dirtyFlag = true;

    XMStoreFloat4x4(&m_projMatrix, XMMatrixPerspectiveFovLH(
        XMConvertToRadians(m_fovY),
        m_aspectRatio,
        m_farPlane, // inverted depth
        m_nearPlane));

    UpdateMatrices();
}

// Call it after camera's position or basis was changed.
// Better to call it once per frame before draw call instead of after each add/set function
void Camera::UpdateMatrices()
{
    if (m_dirtyFlag)
    {
        XMStoreFloat4x4(&m_viewMatrix, XMMatrixLookToLH(
            XMLoadFloat3(&m_position),
            XMLoadFloat3(&m_forward),
            XMLoadFloat3(&m_up)));

        XMStoreFloat4x4(&m_viewProjMatrix, XMMatrixMultiply(
            XMLoadFloat4x4(&m_viewMatrix),
            XMLoadFloat4x4(&m_projMatrix)));

        XMStoreFloat4x4(&m_invViewProjMatrix,
            XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewProjMatrix)));

        m_dirtyFlag = false;
    }
}

XMMATRIX Camera::GetViewProjMatrix()
{
    return XMLoadFloat4x4(&m_viewProjMatrix);
}

void Camera::SetPosition(float x, float y, float z)
{
    m_dirtyFlag = true;

    m_position.x = x;
    m_position.y = y;
    m_position.z = z;

    // UpdateMatrices()
}

void Camera::AddPosition(float x, float y, float z)
{
    m_dirtyFlag = true;

    m_position.x += x;
    m_position.y += y;
    m_position.z += z;

    // UpdateMatrices()
}

void Camera::AddRotation(float pitch, float yaw, float roll)
{
    m_dirtyFlag = true;

    float pitchRadians = XMConvertToRadians(pitch); // x-axis
    float yawRadians = XMConvertToRadians(yaw); // y-axis
#ifdef ENABLE_CAMERA_ROLL
    float rollRadians = XMConvertToRadians(roll); // z-axis
#endif

    XMVECTOR quaternionPitch = XMQuaternionRotationAxis(XMLoadFloat3(&m_right), pitchRadians);
#ifdef ENABLE_CAMERA_ROLL
    XMVECTOR quaternionYaw = XMQuaternionRotationAxis(XMLoadFloat3(&m_up), yawRadians);
    XMVECTOR quaternionRoll = XMQuaternionRotationAxis(XMLoadFloat3(&m_forward), rollRadians);
#else
    // rotate around world's y-axis if roll is disabled to avoid camera tilt (zavalivanie)
    XMFLOAT3 up(0.0f, 1.0f, 0.0f);
    XMVECTOR quaternionYaw = XMQuaternionRotationAxis(XMLoadFloat3(&up), yawRadians);
#endif

    // yaw -> pitch -> roll
#ifdef ENABLE_CAMERA_ROLL
    XMVECTOR quaternion = XMQuaternionMultiply(XMQuaternionMultiply(quaternionYaw, quaternionPitch), quaternionRoll);
#else
    // yaw -> pitch order still causes small camera tilt for some reason:
    // XMVECTOR quaternion = XMQuaternionMultiply(quaternionYaw, quaternionPitch);
    // 
    // BTW XMMatrixRotationRollPitchYaw() order is roll -> pitch -> yaw
    // As far as I know, the order depends only on which axis the gimbal lock will be
    XMVECTOR quaternion = XMQuaternionMultiply(quaternionPitch, quaternionYaw);
#endif

    XMStoreFloat3(&m_forward, XMVector3Rotate(XMLoadFloat3(&m_forward), quaternion));
    XMStoreFloat3(&m_up, XMVector3Rotate(XMLoadFloat3(&m_up), quaternion));
    // calculate m_right as cross product:
    XMStoreFloat3(&m_right, XMVector3Cross(XMLoadFloat3(&m_up), XMLoadFloat3(&m_forward)));

    // UpdateMatrices()
}

XMVECTOR Camera::GetPosition() const
{
    return XMLoadFloat3(&m_position);
}

XMVECTOR Camera::GetRight() const
{
    return XMLoadFloat3(&m_right);
}

XMVECTOR Camera::GetUp() const
{
    return XMLoadFloat3(&m_up);
}

XMVECTOR Camera::GetForward() const
{
    return XMLoadFloat3(&m_forward);
}

XMVECTOR Camera::Reproject(float x, float y) const
{
    // point on near plane (reversed depth: z = 1.0f):
    XMFLOAT4 pointCS(x, y, 1.0f, 1.0f);

    XMVECTOR pointWS = XMVector4Transform(XMLoadFloat4(&pointCS), XMLoadFloat4x4(&m_invViewProjMatrix));
    XMVECTOR w = XMVectorSplatW(pointWS); // pointWS.wwww
    pointWS = XMVectorDivide(pointWS, w);

    return pointWS;
}
} // namespace engine
