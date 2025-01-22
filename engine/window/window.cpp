#include "window.h"

#include <cassert>

namespace engine
{
Window::Window(uint16_t width, uint16_t height)
    : m_window(nullptr)
    , m_screenSurface(nullptr)
    , m_width(width)
    , m_height(height)
    , m_swapChain(nullptr)
    , m_backBuffer(nullptr)
    , m_renderTarget(nullptr)
{

}

void Window::Create()
{
    m_window = SDL_CreateWindow(
        "SDL + Direct3D11",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_width, // client area size
        m_height,
        SDL_WINDOW_SHOWN);
    assert(m_window && "Failed to create window\n");

    m_screenSurface = SDL_GetWindowSurface(m_window);

    CreateSwapchain();
}

void Window::CreateSwapchain()
{
    Globals *globals = Globals::GetInstance();

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
    swapChainDesc.Flags = {};

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc = {};
    swapChainFullscreenDesc.Windowed = true;

    // From docs: caller must initialize the info structure's version before SDL_GetWindowWMInfo()
    SDL_SysWMinfo winInfo;
    SDL_VERSION(&winInfo.version);
    assert(SDL_GetWindowWMInfo(m_window, &winInfo) == SDL_TRUE && "Failed to get HWND\n");

    HRESULT hr = globals->m_dxgiFactory->CreateSwapChainForHwnd(
        globals->m_device.Get(),
        winInfo.info.win.window,
        &swapChainDesc,
        &swapChainFullscreenDesc,
        nullptr,
        &m_swapChain);
    assert(hr >= 0 && "Failed to create swapchain\n");

    // Swapchain resources
    hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&m_backBuffer));
    assert(hr >= 0 && "Failed to get Back Buffer from the SwapChain\n");


    hr = globals->m_device->CreateRenderTargetView(
        m_backBuffer.Get(),
        nullptr,
        &m_renderTarget);
    assert(hr >= 0 && "Failed to create RTV from Back Buffer\n");

    // Create Viewport
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(m_width);
    viewport.Height = static_cast<float>(m_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
}

void Window::Destroy()
{
    SDL_DestroyWindow(m_window); // SDL_CreateWindow
}

void Window::Update()
{
    // fill window with green color
    //SDL_FillRect(m_screenSurface, NULL, SDL_MapRGB(m_screenSurface->format, 0, 255, 0));
    //SDL_UpdateWindowSurface(m_window);

    Globals *globals = Globals::GetInstance();

    constexpr float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };

    globals->m_deviceContext->RSSetViewports(1, &viewport);
    globals->m_deviceContext->ClearRenderTargetView(m_renderTarget.Get(), clearColor);
    globals->m_deviceContext->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);
    m_swapChain->Present(1, 0);
}
} // namespace engine