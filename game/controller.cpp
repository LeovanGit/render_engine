#include "controller.h"

Controller::Controller(std::shared_ptr<engine::Camera> camera)
    : m_camera(camera)
    , m_movePower(1.0f)
    , m_rotatePower(12.0f)
{
    m_keyStates = SDL_GetKeyboardState(nullptr);
}

void Controller::Update(float deltaTime)
{
    SDL_PumpEvents();

    if (m_keyStates[SDL_SCANCODE_W])
    {
        DirectX::XMFLOAT3 forward;
        DirectX::XMStoreFloat3(&forward,
            DirectX::XMVectorScale(m_camera->GetForward(), m_movePower * deltaTime));

        m_camera->AddPosition(forward.x, forward.y, forward.z);
    }

    if (m_keyStates[SDL_SCANCODE_S])
    {
        DirectX::XMFLOAT3 backward;
        DirectX::XMStoreFloat3(&backward,
            DirectX::XMVectorScale(m_camera->GetForward(), -m_movePower * deltaTime));

        m_camera->AddPosition(backward.x, backward.y, backward.z);
    }

    if (m_keyStates[SDL_SCANCODE_D])
    {
        DirectX::XMFLOAT3 right;
        DirectX::XMStoreFloat3(&right,
            DirectX::XMVectorScale(m_camera->GetRight(), m_movePower * deltaTime));

        m_camera->AddPosition(right.x, right.y, right.z);
    }

    if (m_keyStates[SDL_SCANCODE_A])
    {
        DirectX::XMFLOAT3 right;
        DirectX::XMStoreFloat3(&right,
            DirectX::XMVectorScale(m_camera->GetRight(), -m_movePower * deltaTime));

        m_camera->AddPosition(right.x, right.y, right.z);
    }

    if (m_keyStates[SDL_SCANCODE_Q])
    {
        DirectX::XMFLOAT3 up;
        DirectX::XMStoreFloat3(&up,
            DirectX::XMVectorScale(m_camera->GetUp(), m_movePower * deltaTime));

        m_camera->AddPosition(up.x, up.y, up.z);
    }

    if (m_keyStates[SDL_SCANCODE_E])
    {
        DirectX::XMFLOAT3 up;
        DirectX::XMStoreFloat3(&up,
            DirectX::XMVectorScale(m_camera->GetUp(), -m_movePower * deltaTime));

        m_camera->AddPosition(up.x, up.y, up.z);
    }

    int currentMouseX = 0;
    int currentMouseY = 0;
    uint32_t mouseState = SDL_GetMouseState(&currentMouseX, &currentMouseY);

    int deltaMouseX = 0;
    int deltaMouseY = 0;
    SDL_GetRelativeMouseState(&deltaMouseX, &deltaMouseY);

    if (mouseState & SDL_BUTTON_LMASK)
    {
        m_camera->AddRotation(
            deltaMouseY * m_rotatePower * deltaTime,
            deltaMouseX * m_rotatePower * deltaTime,
            0.0f);
    }
}