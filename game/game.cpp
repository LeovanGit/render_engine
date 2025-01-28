#include <SDL.h>

#include <iostream>
#include <cassert>

#include "render/engine.h"
#include "window/window.h"
#include "render/renderer.h"

#include <DirectXMath.h>

struct VertexBufferGPU
{
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_color;
    DirectX::XMFLOAT2 m_uv;
};

void InitScene(engine::Renderer &renderer)
{
    engine::ShaderManager *sm = engine::ShaderManager::GetInstance();
    engine::TextureManager *tm = engine::TextureManager::GetInstance();
    engine::ModelManager *mm = engine::ModelManager::GetInstance();

    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT, // float3
            0,
            D3D11_APPEND_ALIGNED_ELEMENT, //offsetof(VertexBufferGPU, position), 0
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT, // float3
            0,
            D3D11_APPEND_ALIGNED_ELEMENT, //offsetof(VertexBufferGPU, color), 12
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "UV",
            0,
            DXGI_FORMAT_R32G32_FLOAT, // float2
            0,
            D3D11_APPEND_ALIGNED_ELEMENT, //offsetof(VertexBufferGPU, color), 24
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    renderer.m_currentShader = sm->GetOrCreateShader(
        engine::ShaderStage_VertexShader | engine::ShaderStage_PixelShader,
        L"../assets/shaders/hello_triangle.hlsl",
        inputLayout,
        _countof(inputLayout));

    VertexBufferGPU vertices[] =
    {
        // vertex 1
        {
            DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), // pos
            DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), // color
            DirectX::XMFLOAT2(0.0f, 0.0f) // uv
        },
        // vertex 2
        {
            DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f),
            DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
            DirectX::XMFLOAT2(1.0f, 1.0f)
        },
        // vertex 3
        {
            DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f),
            DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),
            DirectX::XMFLOAT2(1.0f, 0.0f)
        },
        // vertex 4
        {
            DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f),
            DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
            DirectX::XMFLOAT2(0.0f, 0.0f)
        },
        // vertex 5
        {
            DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f),
            DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
            DirectX::XMFLOAT2(0.0f, 1.0f)
        },
        // vertex 6
        {
            DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f),
            DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),
            DirectX::XMFLOAT2(1.0f, 1.0f)
        }
    };

    renderer.m_currentVertexBuffer = mm->GetOrCreateModel(
        "triangle",
        vertices,
        sizeof(vertices),
        sizeof(VertexBufferGPU));

    renderer.m_currentTexture = tm->GetOrCreateTexture(L"../assets/textures/default.dds");
}

int main(int argc, char *argv[])
{
    engine::Engine::Create();
    engine::Engine *engine = engine::Engine::GetInstance();

    engine->Init();

    engine::Window window(640, 480);
    window.Create();

    engine::Renderer renderer(&window);
    InitScene(renderer);

    // TODO: hide SDL under engine.vcxproj
    SDL_Event event;
    bool run = true;
    while (run)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT) run = false;

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    SDL_Log("up\n");
                    break;
                case SDLK_DOWN:
                    SDL_Log("down\n");
                    break;
                case SDLK_RIGHT:
                    SDL_Log("right\n");
                    break;
                case SDLK_LEFT:
                    SDL_Log("left\n");
                    break;
                default:
                    SDL_Log("unknown\n");
                    break;
                }
            }
        }
        
        renderer.Render();
    }

    window.Destroy();
    renderer.Destroy(); // unbind all resources
    engine->Deinit();
    engine::Engine::Destroy();

    return 0;
}
