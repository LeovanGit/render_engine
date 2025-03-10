#pragma once

#include <SDL.h>
#include <SDL_syswm.h>

#include "graphics_context/globals.h"

namespace engine
{
class Window
{
public:
    Window(uint16_t width, uint16_t height);
    ~Window() = default;

    void Create(uint16_t posX, uint16_t posY);
    void Destroy();

    void Resize(uint16_t newWidth, uint16_t newHeight);

    void SetViewport();
    void SetScissorRect();

    void ClearRenderTarget();
    void BindRenderTarget();

    void Present();

    void SetTitle(const char *title);


    uint16_t GetWidth() const
    {
        return m_width;
    }

    uint16_t GetHeight() const
    {
        return m_height;
    }

    ComPtr<ID3D12Resource> GetCurrentBackbuffer() const
    {
        return m_swapchainBuffers[m_currentBackbuffer];
    }

private:
    void CreateSwapchain();
    void CreateRenderTargetView();
    void CreateDepthStencilView();

    SDL_Window *m_window;

    uint16_t m_width; // client area size
    uint16_t m_height;

    uint32_t m_currentBackbuffer;

    ComPtr<IDXGISwapChain1> m_swapchain;
    ComPtr<ID3D12Resource> m_swapchainBuffers[s_swapchainBuffersCount];
    ComPtr<ID3D12Resource> m_depthBuffer;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
};
} // namespace engine
