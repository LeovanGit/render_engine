#include "controller.h"

#include <string>

#include <DirectXMath.h>

Controller::Controller()
    : m_window(nullptr)
    , m_renderer(nullptr)
    , m_moveSpeed(1.0f)
    , m_rotateSpeed(30.0f)
    , m_drawDebug(false)
{
    m_keyStates = SDL_GetKeyboardState(nullptr);

    // Create Window:
    m_window = std::make_shared<engine::Window>(1280, 720);
    m_window->Create(500, 200);

    // Create Renderer:
    m_renderer = std::make_shared<engine::Renderer>(m_window);
}

void Controller::InitScene()
{
    m_renderer->m_camera = std::make_unique<engine::Camera>(
        m_renderer->m_window->GetWidth(),
        m_renderer->m_window->GetHeight(),
        0.1f,
        1000.0f,
        90.0f);
    m_renderer->m_camera->SetPosition(0.0f, 0.0f, 0.0f);

    engine::ModelManager *mm = engine::ModelManager::GetInstance();
    m_renderer->m_opaqueInstances->AddInstance(
        L"../assets/textures/bricks.dds",
        mm->GenerateUnitCube("unitCube"),
        { 0.0f, 0.0f, 3.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 45.0f, 0.0f });

    m_renderer->m_opaqueInstances->AddInstance(
        L"../assets/textures/bricks.dds",
        mm->GenerateUnitCube("unitCube"),
        { 0.0f, 2.0f, 3.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f });

    m_renderer->m_opaqueInstances->AddInstance(
        L"../assets/textures/lava.dds",
        mm->GenerateUnitCube("worldOrigin"),
        { 3.0f, 0.0f, 3.0f },
        { 0.5f, 0.5f, 0.5f },
        { 0.0f, 0.0f, 0.0f });

    m_renderer->m_opaqueInstances->AddInstance(
        L"../assets/textures/lava.dds",
        mm->GenerateUnitCube("worldOrigin"),
        { 3.0f, 1.0f, 3.0f },
        { 0.5f, 0.5f, 0.5f },
        { 0.0f, 32.0f, 0.0f });

    m_renderer->m_opaqueInstances->AddInstance(
        L"../assets/textures/lava.dds",
        mm->GenerateUnitCube("worldOrigin"),
        { 0.0f, 0.0f, 0.0f },
        { 0.05f, 0.05f, 0.05f },
        { 0.0f, 0.0f, 0.0f });

    /*// be aware: for big grid size need to change index type from uint16 to uin32
    // max for uint16 is 21845 triangles
    m_renderer->m_terrain = std::make_shared<engine::Terrain>(
        L"../assets/textures/terrain_heightmap.dds",
        80,
        120,
        DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f),
        DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f),
        DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f));*/

    m_renderer->m_skybox = std::make_shared<engine::Sky>(L"../assets/textures/skybox.dds");

    /*m_renderer->m_postprocess = std::make_shared<engine::PostProcess>();

    m_renderer->m_quadSphere = std::make_shared<engine::QuadSphere>(
        L"../assets/textures/default.dds",
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
        DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f),
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));*/
}

void Controller::Update(float deltaTime, uint32_t fps)
{
    UpdateScene(deltaTime);
    m_renderer->m_camera->UpdateMatrices();

    std::string title = "Render Engine (SDL + Direct3D12). FPS: " + std::to_string(fps);
    m_window->SetTitle(title.c_str());
}

void Controller::UpdateScene(float deltaTime)
{
    SDL_PumpEvents();

    float acceleration = m_keyStates[SDL_SCANCODE_LSHIFT] ? 4.0f : 1.0f;

    if (m_keyStates[SDL_SCANCODE_W])
    {
        DirectX::XMFLOAT3 forward;
        DirectX::XMStoreFloat3(&forward,
            DirectX::XMVectorScale(m_renderer->m_camera->GetForward(), m_moveSpeed * acceleration * deltaTime));

        m_renderer->m_camera->AddPosition(forward.x, forward.y, forward.z);
    }

    if (m_keyStates[SDL_SCANCODE_S])
    {
        DirectX::XMFLOAT3 backward;
        DirectX::XMStoreFloat3(&backward,
            DirectX::XMVectorScale(m_renderer->m_camera->GetForward(), -m_moveSpeed * acceleration * deltaTime));

        m_renderer->m_camera->AddPosition(backward.x, backward.y, backward.z);
    }

    if (m_keyStates[SDL_SCANCODE_D])
    {
        DirectX::XMFLOAT3 right;
        DirectX::XMStoreFloat3(&right,
            DirectX::XMVectorScale(m_renderer->m_camera->GetRight(), m_moveSpeed * acceleration * deltaTime));

        m_renderer->m_camera->AddPosition(right.x, right.y, right.z);
    }

    if (m_keyStates[SDL_SCANCODE_A])
    {
        DirectX::XMFLOAT3 right;
        DirectX::XMStoreFloat3(&right,
            DirectX::XMVectorScale(m_renderer->m_camera->GetRight(), -m_moveSpeed * acceleration * deltaTime));

        m_renderer->m_camera->AddPosition(right.x, right.y, right.z);
    }

    if (m_keyStates[SDL_SCANCODE_Q])
    {
        DirectX::XMFLOAT3 up;
        DirectX::XMStoreFloat3(&up,
            DirectX::XMVectorScale(m_renderer->m_camera->GetUp(), m_moveSpeed * acceleration * deltaTime));

        m_renderer->m_camera->AddPosition(up.x, up.y, up.z);
    }

    if (m_keyStates[SDL_SCANCODE_E])
    {
        DirectX::XMFLOAT3 up;
        DirectX::XMStoreFloat3(&up,
            DirectX::XMVectorScale(m_renderer->m_camera->GetUp(), -m_moveSpeed * acceleration * deltaTime));

        m_renderer->m_camera->AddPosition(up.x, up.y, up.z);
    }

    // mouse coords relative to window client area:
    int mouseX = 0;
    int mouseY = 0;
    uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseState & SDL_BUTTON_RMASK)
    {
        float screenWidthHalf = float(m_window->GetWidth()) / 2.0f;
        float screenHeightHalf = float(m_window->GetHeight()) / 2.0f;

        float screenCenterX = screenWidthHalf;
        float screenCenterY = screenHeightHalf;

        float deltaX = mouseX - screenCenterX;
        float deltaY = mouseY - screenCenterY;

        // rotate speed depends on the mouse distance from the center of the screen:
        // deltaX = screenWidthHalf is equal to 180 degrees per second speed
        // deltaY = screenHeightHalf is equal to 180 degrees per second speed
        float rotateSpeedX = 180.0f * deltaX / screenWidthHalf;
        float rotateSpeedY = 180.0f * deltaY / screenHeightHalf;

        // To reach the same rotation for different deltaTimes we should
        // pass degreesPerSecond * deltaTime to Camera::AddRotation()
        m_renderer->m_camera->AddRotation(
            rotateSpeedY * deltaTime,
            rotateSpeedX * deltaTime,
            0.0f);
    }
}

void Controller::Draw()
{
    m_renderer->Render(m_drawDebug);
}

void Controller::OnWindowResize(uint32_t width, uint32_t height)
{
    m_window->Resize(width, height);

    m_renderer->m_camera->ChangeSettings(
        width,
        height,
        m_renderer->m_camera->GetNearPlane(),
        m_renderer->m_camera->GetFarPlane(),
        m_renderer->m_camera->GetFOVY());
}

void Controller::Destroy()
{
    m_window->Destroy();
    m_renderer->Destroy();
}
