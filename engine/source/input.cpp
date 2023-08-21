#include "input.h"

Input::Input() : m_mouse_position(0, 0)
{
    uint8_t keys_count = static_cast<uint8_t>(VK_CODE::MAX);
    for (uint8_t i = 0; i != keys_count; ++i) m_keys_state[i] = false;
}

void Input::PressKey(VK_CODE key)
{
    m_keys_state[static_cast<uint8_t>(key)] = true;
}

void Input::ReleaseKey(VK_CODE key)
{
    m_keys_state[static_cast<uint8_t>(key)] = false;
}

bool Input::IsKeyPressed(VK_CODE key) const
{
    return m_keys_state[static_cast<uint8_t>(key)];
}

void Input::CaptureMousePosition(uint16_t pos_x, uint16_t pos_y)
{
    m_mouse_position.x = pos_x;
    m_mouse_position.y = pos_y;
}
