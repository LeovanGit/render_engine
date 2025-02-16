#include <SDL.h>

#include <iostream>
#include <cassert>
#include <string>

#include "render/engine.h"
#include "window/window.h"
#include "render/renderer.h"
#include "timer/timer.h"

#include <DirectXMath.h>

#include "controller.h"

void InitScene(engine::Renderer &renderer)
{
    engine::ShaderManager *sm = engine::ShaderManager::GetInstance();
    engine::ModelManager *mm = engine::ModelManager::GetInstance();
    engine::TextureManager *tm = engine::TextureManager::GetInstance();

    renderer.m_camera = std::make_unique<engine::Camera>(
        renderer.m_window->GetWidth(),
        renderer.m_window->GetHeight(),
        0.1f,
        1000.0f,
        90.0f);
    renderer.m_camera->SetPosition(0.0f, 0.0f, 0.0f);

    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT, // float3
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "UV",
            0,
            DXGI_FORMAT_R32G32_FLOAT, // float2
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    renderer.m_currentShader = sm->GetOrCreateShader(
        engine::ShaderStage_VertexShader |
        engine::ShaderStage_HullShader |
        engine::ShaderStage_DomainShader | 
        engine::ShaderStage_PixelShader,
        L"../assets/shaders/opaque.hlsl",
        inputLayout,
        _countof(inputLayout));

    renderer.m_debugShader = sm->GetOrCreateShader(
        engine::ShaderStage_VertexShader |
        engine::ShaderStage_GeometryShader |
        engine::ShaderStage_PixelShader,
        L"../assets/shaders/debug.hlsl",
        inputLayout,
        _countof(inputLayout));

    renderer.m_meshes.push_back(mm->GenerateCube(
        "unitCube",
        L"../assets/textures/bricks.dds",
        { 0.0f, 0.0f, 3.0f },
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 45.0f, 0.0f }));

    renderer.m_meshes.push_back(mm->GenerateCube(
        "worldOrigin",
        L"../assets/textures/lava.dds",
        { 0.0f, 0.0f, 0.0f },
        { 0.02f, 0.02f, 0.02f },
        { 0.0f, 0.0f, 0.0f }));

    // be aware: for big grid size need to change index type from uint16 to uin32
    // max for uint16 is 21845 triangles
    renderer.m_terrain = std::make_shared<engine::Terrain>(
        L"../assets/textures/terrain_heightmap.dds",
        80,
        120,
        DirectX::XMFLOAT3(0.0f, -10.0f, 0.0f),
        DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f),
        DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f));

    renderer.m_skybox = std::make_shared<engine::Sky>(L"../assets/textures/skybox.dds");

    renderer.m_computeShader = sm->GetOrCreateShader(
        engine::ShaderStage_ComputeShader,
        L"../assets/shaders/compute.hlsl");

    renderer.m_computeTexture = tm->GetOrCreateTexture(L"RWTexture", true);

    uint32_t readBufferData[] =
    {
        10, 20, 30, 40, 50, 60, 70, 80, 90
    };

    renderer.m_readBuffer = std::make_shared<engine::Buffer>(
        readBufferData,
        sizeof(readBufferData),
        sizeof(uint32_t),
        engine::BufferUsage::BufferUsage_ReadBuffer);

    struct BufferData
    {
        float value0;
        float value1;
    };

    BufferData readStructuredBufferData[] =
    {
        { 0.0f, 1.0f },
        { 3.0f, 2.0f },
        { -5.0f, 11.0f }
    };

    renderer.m_readStructuredBuffer = std::make_shared<engine::Buffer>(
        readStructuredBufferData,
        sizeof(readStructuredBufferData),
        sizeof(BufferData),
        engine::BufferUsage::BufferUsage_ReadBuffer,
        true);
}

int main(int argc, char *argv[])
{
    engine::Engine::Create();
    engine::Engine *engine = engine::Engine::GetInstance();

    engine->Init();

    engine::Window window(1280, 720);
    window.Create();

    engine::Renderer renderer(&window);
    InitScene(renderer);

    Controller controller(renderer.m_camera);

    engine::FPSTimer fpsTimer(0);
    fpsTimer.Start();

    bool debugMode = false;
    bool keyNDown = false; // could be std::map with event.key.keysym.sym as keys

    // TODO: hide SDL under engine.vcxproj
    SDL_Event event;
    bool run = true;
    while (run)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
            {
                run = false;
                break;
            }
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_n && !keyNDown)
                {
                    debugMode = !debugMode;
                    keyNDown = true;
                }
                break;
            }
            case SDL_KEYUP:
            {
                if (event.key.keysym.sym == SDLK_n && keyNDown)
                {
                    keyNDown = false;
                }
                break;
            }
            case SDL_MOUSEWHEEL:
            {
                float newMoveSpeed = controller.m_moveSpeed + event.wheel.y;
                controller.m_moveSpeed = newMoveSpeed > 0 ? newMoveSpeed : 0.1f;
                break;
            }
            }
        }
        
        if (fpsTimer.IsTimeElapsed())
        {
            controller.Update(fpsTimer.GetDeltaTime());
            renderer.m_camera->UpdateMatrices();
            renderer.Render(debugMode);

            std::string title = "SDL + Direct3D11 Engine. FPS: " + std::to_string(fpsTimer.GetFPS());
            window.SetTitle(title.c_str());
        }
    }

    window.Destroy();
    renderer.Destroy(); // unbind all resources
    engine->Deinit();
    engine::Engine::Destroy();

    return 0;
}
