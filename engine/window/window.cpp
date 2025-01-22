#include "window.h"

#include <cassert>

namespace engine
{
Window::Window(uint16_t width, uint16_t height)
    : m_window(nullptr)
    , m_screenSurface(nullptr)
    , m_width(width)
    , m_height(height)
{

}

void Window::Create()
{
    m_window = SDL_CreateWindow(
        "SDL + Direct3D11",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_width,
        m_height,
        SDL_WINDOW_SHOWN);
    assert(m_window && "Failed to create window");

    m_screenSurface = SDL_GetWindowSurface(m_window);
}

void Window::Destroy()
{
    SDL_DestroyWindow(m_window); // SDL_CreateWindow
}

void Window::Update()
{
    // fill window with green color
    SDL_FillRect(m_screenSurface, NULL, SDL_MapRGB(m_screenSurface->format, 0, 255, 0));
    SDL_UpdateWindowSurface(m_window);
}
} // namespace engine