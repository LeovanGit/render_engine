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

    uint16_t GetWidth() const { return m_width; }
    uint16_t GetHeight() const { return m_height; }

    void Resize(uint16_t newWidth, uint16_t newHeight);

    void SetViewport();

    void ClearRenderTarget();
    void BindRenderTarget();
    void UnbindRenderTarget();

    void Present();

    void SetTitle(const char *title);

private:
    void CreateSwapchain();
    void CreateRenderTargetView();
    void CreateDepthStencilView();

    SDL_Window *m_window;

    uint16_t m_width; // client area size
    uint16_t m_height;

    ComPtr<IDXGISwapChain1> m_swapchain;
    ComPtr<ID3D11RenderTargetView> m_renderTarget;
    ComPtr<ID3D11DepthStencilView> m_depthStencil;

    D3D11_VIEWPORT m_viewport;
};
} // namespace engine