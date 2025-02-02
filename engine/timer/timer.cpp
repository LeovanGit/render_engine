#include "timer.h"

namespace
{
constexpr uint32_t MILLISECONDS_IN_SECOND = 1000.0f;
}

namespace engine
{
FPSTimer::FPSTimer(uint8_t fpsCap)
    : m_fpsCap(fpsCap)
    , m_prevTime(0)
    , m_deltaTime(0)
{
    if (m_fpsCap != 0)
        m_msCap = MILLISECONDS_IN_SECOND / m_fpsCap;
    else
        m_msCap = 0; // no limit
}

void FPSTimer::Start()
{
    m_prevTime = SDL_GetTicks();
}

bool FPSTimer::IsTimeElapsed()
{
    // TODO: SDL_GetTicks has very poor accuracy, use std::chrono instead
    uint32_t currentTime = SDL_GetTicks();

    uint32_t delta = currentTime - m_prevTime;
    if (delta < m_msCap) return false;

    m_prevTime = currentTime;
    // if deltaTime is 0, then set it to 1 ms to avoid division zero:
    m_deltaTime = delta == 0 ? 1 : delta;
}

uint8_t FPSTimer::GetFPS() const
{
    return MILLISECONDS_IN_SECOND / m_deltaTime;
}

float FPSTimer::GetDeltaTime() const
{
    // in seconds
    return static_cast<float>(m_deltaTime) / MILLISECONDS_IN_SECOND;
}
} // namespace engine