#pragma once

#include <SDL.h>
#include <SDL_syswm.h>

#include "render/globals.h"

namespace engine
{
class Window
{
public:
    Window(uint16_t width, uint16_t height);
    ~Window() = default;
    
    void Create();
    void Destroy();

    uint16_t GetWidth() { return m_width; }
    uint16_t GetHeight() { return m_height; }

    void SetViewport();
    void ClearRenderTarget();
    void SetRenderTarget();
    void Present();

    void SetTitle(const char *title);

private:
    void CreateSwapchain();

    SDL_Window *m_window;
    SDL_Surface *m_screenSurface;

    uint16_t m_width;
    uint16_t m_height;

    ComPtr<IDXGISwapChain1> m_swapchain;
    ComPtr<ID3D11Texture2D> m_backbuffer;
    ComPtr<ID3D11RenderTargetView> m_renderTarget;

    D3D11_VIEWPORT viewport;
};
} // namespace engine