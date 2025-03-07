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

    m_renderer->m_mesh = mm->GenerateCube("unitCube",
        L"../assets/textures/bricks.dds",
        { 0.0f, 0.0f, 3.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 45.0f, 0.0f });

    struct InstanceBufferData
    {
        DirectX::XMFLOAT4X4 modelMatrix;
    } ibData;
    DirectX::XMStoreFloat4x4(&ibData.modelMatrix, m_renderer->m_mesh->m_modelMatrix);

    m_renderer->m_instanceBuffer = std::make_shared<engine::Buffer>(
        &ibData.modelMatrix,
        sizeof(InstanceBufferData),
        sizeof(InstanceBufferData),
        engine::BufferUsage::BufferUsage_InstanceBuffer);

    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "UV",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "TRANSFORM",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            2,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
        {
            "TRANSFORM",
            3,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            1,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            1
        },
    };

    engine::ShaderManager *sm = engine::ShaderManager::GetInstance();
    m_renderer->m_shader = sm->GetOrCreateShader(
        ShaderStage_VertexShader | ShaderStage_PixelShader,
        L"../assets/shaders/opaque.hlsl",
        inputLayout,
        _countof(inputLayout));

    engine::Globals *globals = engine::Globals::GetInstance();
    globals->CreateRootSignature();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc = {};
    PSODesc.pRootSignature = globals->m_globalRootSignature.Get();
    PSODesc.VS =
    {
        m_renderer->m_shader->m_VSBytecode->GetBufferPointer(),
        m_renderer->m_shader->m_VSBytecode->GetBufferSize()
    };
    PSODesc.PS =
    {
        m_renderer->m_shader->m_PSBytecode->GetBufferPointer(),
        m_renderer->m_shader->m_PSBytecode->GetBufferSize()
    };
    PSODesc.BlendState = globals->m_blendState;
    PSODesc.SampleMask = UINT_MAX;
    PSODesc.RasterizerState = globals->m_rasterizerState;
    PSODesc.DepthStencilState = globals->m_depthStencilState;
    PSODesc.InputLayout = m_renderer->m_shader->m_inputLayout;
    PSODesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSODesc.NumRenderTargets = 1;
    PSODesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    PSODesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    PSODesc.SampleDesc.Count = 1;
    PSODesc.SampleDesc.Quality = 0;
    PSODesc.NodeMask = 0;
    PSODesc.CachedPSO = { nullptr, 0 };
    PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    engine::PipelineManager *pm = engine::PipelineManager::GetInstance();
    pm->GetOrCreatePipeline("opaque", &PSODesc);

    /*m_renderer->m_opaqueInstances->AddInstance(mm->GenerateCube(
        "unitCube",
        L"../assets/textures/bricks.dds",
        { 0.0f, 0.0f, 3.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 45.0f, 0.0f }));

    m_renderer->m_opaqueInstances->AddInstance(mm->GenerateCube(
        "worldOrigin",
        L"../assets/textures/lava.dds",
        { 3.0f, 0.0f, 3.0f },
        { 0.5f, 0.5f, 0.5f },
        { 0.0f, 0.0f, 0.0f }));

    // be aware: for big grid size need to change index type from uint16 to uin32
    // max for uint16 is 21845 triangles
    m_renderer->m_terrain = std::make_shared<engine::Terrain>(
        L"../assets/textures/terrain_heightmap.dds",
        80,
        120,
        DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f),
        DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f),
        DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f));

    m_renderer->m_skybox = std::make_shared<engine::Sky>(L"../assets/textures/skybox.dds");

    m_renderer->m_postprocess = std::make_shared<engine::PostProcess>();

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
