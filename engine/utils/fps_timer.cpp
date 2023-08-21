#include "fps_timer.h"

FPSTimer::FPSTimer(float fps_limit) :
                   m_fps_limit(fps_limit),
                   m_prev_time(std::chrono::steady_clock::now()),
                   m_delta_time(0.0f)
{
    // uncap fps if fps_limit <= 0
    m_frame_duration = fps_limit > 0 ? Seconds(1.0f / fps_limit) : Seconds(0.0f);
}

bool FPSTimer::IsFrameTimeElapsed()
{
    using namespace std::chrono;

    // should cast to Seconds:
    // https://stackoverflow.com/questions/52421819/does-steady-clocknow-return-seconds
    Seconds elapsed_time = duration_cast<Seconds>(steady_clock::now() - m_prev_time);
    
    if (elapsed_time < m_frame_duration) return false;

    m_delta_time = elapsed_time;
    m_prev_time = steady_clock::now();

    return true;
}

float FPSTimer::GetDeltaTime()
{
    return m_delta_time.count();
}
