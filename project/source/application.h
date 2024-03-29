#pragma once

#include <memory>

#include "windows/window.h"
#include "source/input_listener.h"
#include "source/input.h"
#include "render/scene.h"
#include "source/math/sphere.h"

class Application : public InputListener
{
public:
    Application(HINSTANCE app_handle);

    void InitScene();
    void ProcessInput(float delta_time);

    virtual void onKeyDown(VK_CODE key_code);
    virtual void onKeyUp(VK_CODE key_code);

    virtual void onMouseMove(uint16_t pos_x, uint16_t pos_y);

    virtual ~Application() = default;

    Window m_window;
    Input m_input;
    Scene m_scene;

    float m_speed;
};