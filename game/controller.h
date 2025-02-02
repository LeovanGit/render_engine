#pragma once

#include <SDL.h>

#include <iostream>

#include "render/camera.h"
#include "window/window.h"

class Controller
{
public:
    Controller(std::shared_ptr<engine::Camera> camera);

    void Update(float deltaTime);

    std::shared_ptr<engine::Camera> m_camera;

    const uint8_t *m_keyStates;

    float m_movePower;
    float m_rotatePower;
};