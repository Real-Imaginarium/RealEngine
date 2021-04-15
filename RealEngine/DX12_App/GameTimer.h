#pragma once

#include <stdint.h>

class GameTimer
{
public:
    GameTimer();

    float TotalTime() const;            // sec

    float DeltaTime() const;            // sec

    void Reset();                       // call before message loop

    void Start();                       // call when unpaused

    void Stop();                        // call when paused

    void Tick();                        // call every frame

private:
    double  m_seconds_per_count;
    double  m_delta_time;

    int64_t m_base_time;
    int64_t m_paused_time;
    int64_t m_stop_time;
    int64_t m_prev_time;
    int64_t m_curr_time;

    bool m_stopped;
};
