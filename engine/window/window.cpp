#include "window.h"

#include <cassert>

namespace engine
{
Window::Window(uint16_t width, uint16_t height)
    : m_window(nullptr)
    , m_width(width)
    , m_height(height)
    , m_swapchain(nullptr)
    , m_swapchainBuffersCount(2)
    , m_currentBackbuffer(0)
    , m_depthBuffer(nullptr)
{
    for (uint32_t i = 0; i != m_swapchainBuffersCount; ++i)
    {
        m_swapchainBuffers[i] = nullptr;
    }

    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = static_cast<float>(m_width);
    m_viewport.Height = static_cast<float>(m_height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = m_width;
    m_scissorRect.bottom = m_height;
}

void Window::Create(uint16_t posX, uint16_t posY)
{
    m_window = SDL_CreateWindow(
        "title",
        posX,
        posY,
        m_width,
        m_height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    assert(m_window && "Failed to create window\n");

    CreateSwapchain();
    CreateRenderTargetView();
    CreateDepthStencilView();
}

void Window::CreateSwapchain()
{
    Globals *globals = Globals::GetInstance();

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.Flags = {};

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
    swapChainFullScreenDesc.Windowed = true;

    // From docs: caller must initialize SDL_SysWMinfo::version before SDL_GetWindowWMInfo()
    SDL_SysWMinfo winInfo = {};
    SDL_VERSION(&winInfo.version);
    SDL_bool result = SDL_GetWindowWMInfo(m_window, &winInfo);
    assert(result == SDL_TRUE && "Failed to get HWND\n");

    HRESULT hr = globals->m_dxgiFactory->CreateSwapChainForHwnd(
        globals->m_commandQueue.Get(),
        winInfo.info.win.window,
        &swapChainDesc,
        &swapChainFullScreenDesc,
        nullptr,
        m_swapchain.GetAddressOf());
    assert(hr >= 0 && "Failed to create swapchain\n");
}

// Get backbuffer from swapchain and create RTV for it
void Window::CreateRenderTargetView()
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    for (uint32_t i = 0; i != m_swapchainBuffersCount; ++i)
    {
        hr = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_swapchainBuffers[i]));
        assert(hr >= 0 && "Failed to get buffer from the swapchain\n");

        globals->m_device->CreateRenderTargetView(
            m_swapchainBuffers[i].Get(),
            nullptr,
            globals->GetRTVDescriptor(i));
    }
}

void Window::CreateDepthStencilView()
{
    Globals *globals = Globals::GetInstance();
    HRESULT hr;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Alignment = 0;
    textureDesc.Width = m_width;
    textureDesc.Height = m_height;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 0.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    hr = globals->m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &clearValue,
        IID_PPV_ARGS(m_depthBuffer.GetAddressOf()));
    assert(hr >= 0 && "Failed to create depth texture\n");

    globals->m_device->CreateDepthStencilView(
        m_depthBuffer.Get(),
        nullptr,
        globals->GetDSVDescriptor(0));

    // change state to DEPTH_WRITE:
    globals->BeginCommandsRecording();

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_depthBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);
    globals->m_commandList->ResourceBarrier(1, &barrier);

    globals->EndCommandsRecording();
    globals->Submit();
    globals->FlushCommandQueue();
}

void Window::Destroy()
{
    for (uint32_t i = 0; i != m_swapchainBuffersCount; ++i)
    {
        m_swapchainBuffers[i].Reset();
    }
    
    m_depthBuffer.Reset();
    m_swapchain.Reset();

    SDL_DestroyWindow(m_window); // SDL_CreateWindow
}

void Window::Resize(uint16_t newWidth, uint16_t newHeight)
{
    Globals *globals = Globals::GetInstance();
    globals->FlushCommandQueue();

    m_width = newWidth;
    m_height = newHeight;

    m_viewport.Width = static_cast<float>(newWidth);
    m_viewport.Height = static_cast<float>(newHeight);

    m_scissorRect.right = newWidth;
    m_scissorRect.bottom = newHeight;

    for (uint32_t i = 0; i != m_swapchainBuffersCount; ++i)
    {
        m_swapchainBuffers[i].Reset();
    }

    m_depthBuffer.Reset();

    HRESULT hr = m_swapchain->ResizeBuffers(
        0,
        newWidth,
        newHeight,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        0);

    m_currentBackbuffer = 0;

    CreateRenderTargetView();
    CreateDepthStencilView();
}

void Window::SetViewport()
{
    Globals *globals = Globals::GetInstance();

    globals->m_commandList->RSSetViewports(1, &m_viewport);
}

void Window::SetScissorRect()
{
    Globals *globals = Globals::GetInstance();

    globals->m_commandList->RSSetScissorRects(1, &m_scissorRect);
}

void Window::ClearRenderTarget()
{
    Globals *globals = Globals::GetInstance();

    static constexpr float clearColor0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static constexpr float clearColor1[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_swapchainBuffers[m_currentBackbuffer].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    globals->m_commandList->ResourceBarrier(1, &barrier);

    globals->m_commandList->ClearRenderTargetView(
        globals->GetRTVDescriptor(m_currentBackbuffer),
        m_currentBackbuffer == 0 ? clearColor0 : clearColor1,
        0,
        nullptr);

    // 0.0f - reversed depth
    globals->m_commandList->ClearDepthStencilView(
        globals->GetDSVDescriptor(0),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        0.0f,
        0,
        0,
        nullptr);
}

void Window::BindRenderTarget()
{
    Globals *globals = Globals::GetInstance();

    D3D12_CPU_DESCRIPTOR_HANDLE currentRTV = globals->GetRTVDescriptor(m_currentBackbuffer);
    D3D12_CPU_DESCRIPTOR_HANDLE currentDSV = globals->GetDSVDescriptor(0);

    globals->m_commandList->OMSetRenderTargets(
        1,
        &currentRTV,
        true,
        &currentDSV);

    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GetCurrentBackbuffer().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    globals->m_commandList->ResourceBarrier(1, &barrier);
}

void Window::Present()
{
    m_swapchain->Present(1, 0);

    m_currentBackbuffer = (m_currentBackbuffer + 1) % m_swapchainBuffersCount;
}

void Window::SetTitle(const char *title)
{
    SDL_SetWindowTitle(m_window, title);
}
} // namespace engine
