#pragma once

#include "SDL.h"

namespace engine
{
class FPSTimer
{
public:
    FPSTimer(uint8_t m_fpsCap = 0);

    void Start();

    bool IsTimeElapsed();

    uint8_t GetFPS() const;
    float GetDeltaTime() const;

private:
    uint8_t m_fpsCap;
    uint32_t m_msCap; // milliseconds cap

    uint32_t m_prevTime;

    uint32_t m_deltaTime;
};
} // namespace engine
