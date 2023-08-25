#include "application.h"

Application::Application(HINSTANCE app_handle) :
    m_window(Window(app_handle, 400, 400, 400, 300, 2)),
    m_speed(100.0f)
{
    InitScene();
    m_window.AttachListener(this);
}

void Application::InitScene()
{
    m_scene.AddSphere(math::Sphere(glm::vec3(0, 0, 0), 15));
    m_scene.AddSphere(math::Sphere(glm::vec3(50, 50, 0), 10));
}

void Application::ProcessInput(float delta_time)
{
    if (m_input.IsKeyPressed(VK_CODE::W))
    {
        m_scene.m_spheres[0].m_origin.y += m_speed * delta_time;
    }

    if (m_input.IsKeyPressed(VK_CODE::S))
    {
        m_scene.m_spheres[0].m_origin.y -= m_speed * delta_time;
    }

    if (m_input.IsKeyPressed(VK_CODE::D))
    {
        m_scene.m_spheres[0].m_origin.x += m_speed * delta_time;
    }

    if (m_input.IsKeyPressed(VK_CODE::A))
    {
        m_scene.m_spheres[0].m_origin.x -= m_speed * delta_time;
    }

    if (m_input.IsKeyPressed(VK_CODE::RMB))
    {
        POINT mouse_position;
        GetCursorPos(&mouse_position);

        // to move object on the same distance as mouse moved regardless of the window stretch:
        float stretch_factor = m_window.GetStretchFactor();

        float dx = (mouse_position.x - m_input.m_mouse_position.x) / stretch_factor;
        float dy = (m_input.m_mouse_position.y - mouse_position.y) / stretch_factor;

        m_scene.m_spheres[0].m_origin.x += dx;
        m_scene.m_spheres[0].m_origin.y += dy;

        m_input.CaptureMousePosition(mouse_position.x, mouse_position.y);
    }
}

void Application::onKeyDown(VK_CODE key)
{
    m_input.PressKey(key);

    if (key == VK_CODE::RMB)
    {
        // in screen coordinates (so not depends on window size)
        POINT mouse_position;
        GetCursorPos(&mouse_position);

        m_input.CaptureMousePosition(mouse_position.x, mouse_position.y);
    }
}

void Application::onKeyUp(VK_CODE key)
{
    m_input.ReleaseKey(key);
}

void Application::onMouseMove(uint16_t pos_x, uint16_t pos_y)
{

}