#include "GameTimer.h"

#include <windows.h>


GameTimer::GameTimer()
    : m_seconds_per_count( 0.0 )
    , m_delta_time( -1.0 )
    , m_base_time( 0 )
    , m_paused_time( 0 )
    , m_prev_time( 0 )
    , m_curr_time( 0 )
    , m_stopped( false )
{
    int64_t countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec );
    m_seconds_per_count = 1.0 / (double)countsPerSec;
}


// Returns the total time elapsed since Reset() was called, NOT counting any time when the clock is stopped.
float GameTimer::TotalTime() const
{
    // If we are stopped, do not count the time that has passed since we stopped. Moreover, if we previously already had a pause, the distance m_stop_time - m_base_time
    // includes paused time, which we do not want to count. To correct this, we can subtract the paused time from m_stop_time:  
    //                     |<--paused time-->|
    // ----*---------------*-----------------*------------*------------*------> time
    //  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime
    if( m_stopped )
    {
        return static_cast<float>((( m_stop_time - m_paused_time ) - m_base_time ) * m_seconds_per_count );
    }

    // The distance m_curr_time - mBaseTime includes paused time, which we do not want to count. To correct this, we can subtract the
    // paused time from mCurrTime: (m_curr_time - m_paused_time) - m_base_time 
    //                     |<--paused time-->|
    // ----*---------------*-----------------*------------*------> time
    //  mBaseTime       mStopTime        startTime     mCurrTime
    else
    {
        return static_cast<float>((( m_curr_time - m_paused_time ) - m_base_time ) * m_seconds_per_count );
    }
}


float GameTimer::DeltaTime()const
{
    return static_cast<float>( m_delta_time );
}


void GameTimer::Reset()
{
    int64_t currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime );

    m_base_time = currTime;
    m_prev_time = currTime;
    m_stop_time = 0;
    m_curr_time = false;
}


void GameTimer::Start()
{
    int64_t startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime );

    // Accumulate the time elapsed between stop and start pairs.
    //                     |<-------d------->|
    // ----*---------------*-----------------*------------> time
    //  mBaseTime       mStopTime        startTime     
    if( m_stopped )
    {
        m_paused_time += ( startTime - m_stop_time );
        m_prev_time = startTime;
        m_stop_time = 0;
        m_stopped = false;
    }
}


void GameTimer::Stop()
{
    if( !m_stopped )
    {
        int64_t currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime );

        m_stop_time = currTime;
        m_stopped = true;
    }
}


void GameTimer::Tick()
{
    if( m_stopped )
    {
        m_delta_time = 0.0;
        return;
    }

    int64_t currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime );
    m_curr_time = currTime;
    m_delta_time = ( m_curr_time - m_prev_time ) * m_seconds_per_count;     // Time difference between this frame and the previous.
    m_prev_time = m_curr_time;                                              // Prepare for next frame.

    // Force nonnegative. The DXSDK's CDXUTTimer mentions that if the processor goes into a power save mode or we get shuffled to another processor, then mDeltaTime can be negative.
    if( m_delta_time < 0.0 )
    {
        m_delta_time = 0.0;
    }
}
