#pragma once

#include <cstdint>

#include "key_codes.h"

class InputListener
{
public:
    virtual void onKeyDown(VK_CODE key) = 0;
    virtual void onKeyUp(VK_CODE key) = 0;

    virtual void onMouseMove(uint16_t pos_x, uint16_t pos_y) = 0;

    virtual ~InputListener() = default;
};