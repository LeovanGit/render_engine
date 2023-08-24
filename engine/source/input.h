#pragma once

#include <cstdint>
#include "windows.h"
#include "glm/glm.hpp"

#include "key_codes.h"

class Input
{
public:
    Input();

    void PressKey(VK_CODE key_code);
    void ReleaseKey(VK_CODE key_code);
    bool IsKeyPressed(VK_CODE key_code) const;

    void CaptureMousePosition(uint16_t pos_x, uint16_t pos_y);

    bool m_keys_state[static_cast<uint8_t>(VK_CODE::MAX)];

    glm::u16vec2 m_mouse_position;
};
