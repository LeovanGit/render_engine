#pragma once

#include <SDL.h>

namespace engine
{
class Window
{
public:
    Window(uint16_t width, uint16_t height);
    
    void Create();
    void Destroy();

    void Update();

private:
    SDL_Window *m_window;
    SDL_Surface *m_screenSurface;

    uint16_t m_width;
    uint16_t m_height;
};
} // namespace engine