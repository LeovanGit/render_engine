#pragma once

#include <chrono>

namespace engine
{
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Seconds = std::chrono::duration<float>;

class Timer
{
public:
    Timer();

    bool IsTimeElapsed(float durationInMilliseconds);

    float GetDeltaTime() const;

private:
    TimePoint m_prevTime;
    Seconds m_deltaTime;
};
} // namespace engine
