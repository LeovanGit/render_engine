#include <SDL.h>

#include <iostream>
#include <cassert>

#include "render/engine.h"
#include "window/window.h"
#include "render/renderer.h"

#include "render/shader_manager.h"
#include "render/vertex_buffer.h"
#include <DirectXMath.h>

struct VertexBufferGPU
{
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_color;
};

void InitScene(engine::Renderer &renderer)
{
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
        }
    };

    engine::Shader *vertexShader = new engine::Shader(
        engine::Shader::ShaderType::VertexShader,
        L"../assets/shaders/hello_triangle.hlsl",
        "mainVS",
        inputLayout,
        _countof(inputLayout));
    renderer.SetVertexShader(vertexShader);

    engine::Shader *pixelShader = new engine::Shader(
        engine::Shader::ShaderType::PixelShader,
        L"../assets/shaders/hello_triangle.hlsl",
        "mainPS");
    renderer.SetPixelShader(pixelShader);

    VertexBufferGPU vertices[] =
    {
        { DirectX::XMFLOAT3( 0.0f,  0.5f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // vertex 1
        { DirectX::XMFLOAT3( 0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // vertex 2
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // vertex 3
    };

    engine::VertexBuffer *vertexBuffer = new engine::VertexBuffer(vertices, sizeof(vertices), sizeof(VertexBufferGPU));
    renderer.SetVertexBuffer(vertexBuffer);
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
    engine->Deinit();
    engine::Engine::Destroy();

    return 0;
}
