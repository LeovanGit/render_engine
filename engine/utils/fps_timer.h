#pragma once

#include <chrono>

typedef std::chrono::duration<float> Seconds;
typedef std::chrono::time_point<std::chrono::steady_clock> TimeStamp;

class FPSTimer
{
public:
    FPSTimer(float fps_limit = 60.0f);

    bool IsFrameTimeElapsed();

    float GetDeltaTime();

private:
    float m_fps_limit;
    Seconds m_frame_duration;
    Seconds m_delta_time;

    TimeStamp m_prev_time;
};
