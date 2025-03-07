#include "timer.h"

namespace engine
{
Timer::Timer()
    : m_prevTime(Clock::now())
    , m_deltaTime(0.0f)
{

}

bool Timer::IsTimeElapsed(float durationInSeconds)
{
    using namespace std::chrono;

    TimePoint currentTime = Clock::now();
    auto deltaTime = duration_cast<Seconds>(currentTime - m_prevTime);

    if (deltaTime.count() < durationInSeconds) return false;

    m_prevTime = currentTime;
    m_deltaTime = deltaTime;

    return true;
}

float Timer::GetDeltaTime() const
{
    return m_deltaTime.count();
}
} // namespace engine
