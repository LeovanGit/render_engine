#pragma once

#include <SDL.h>

#include <iostream>

#include "window/window.h"
#include "renderer/renderer.h"
#include "graphics_context/shader_manager.h" // remove

class Controller
{
public:
    Controller();
    ~Controller() = default;

    void InitScene();

    void Update(float deltaTime, uint32_t fps = 0);
    void UpdateScene(float deltaTime);

    void Draw();

    void OnWindowResize(uint32_t width, uint32_t height);

    void Destroy();

    std::shared_ptr<engine::Window> m_window;
    std::shared_ptr<engine::Renderer> m_renderer;

    const uint8_t *m_keyStates;

    float m_moveSpeed;
    float m_rotateSpeed;

    bool m_drawDebug;
};
