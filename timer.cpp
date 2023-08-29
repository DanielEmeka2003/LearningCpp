#include "timer.h"

namespace Myfcn
{
    void Timer::start()
    {
        if (!m_isTimerStarted)
        {
            m_isTimerStarted = true;
            m_isTimerPaused = false;
            m_isTimerZero = false;
            m_pausedTime = {};
            m_startedTime = steady_clock::now().time_since_epoch();
        }
    }
    void Timer::restart()
    {
        if (!m_isTimerZero)
        {
            m_isTimerStarted = true;
            m_isTimerPaused = false;
            m_isTimerZero = false;
            m_pausedTime = {};
            m_startedTime = steady_clock::now().time_since_epoch();    
        }
    }

    void Timer::pause()
    {
        if (!m_isTimerZero and m_isTimerStarted and !m_isTimerPaused)
        {  
            m_isTimerStarted = true;
            m_isTimerPaused = true;
            m_isTimerZero = false;
            m_pausedTime = m_pausedTime + (steady_clock::now().time_since_epoch() - m_startedTime);
            m_startedTime = {};
        }
    }
    void Timer::unpause()
    {
        if (!m_isTimerZero and m_isTimerStarted and m_isTimerPaused)
        {
            m_isTimerPaused = false;
            m_isTimerStarted = true;
            m_isTimerZero = false;
            /* PausedTime remains with its value. */
            m_startedTime = steady_clock::now().time_since_epoch();
        }
    }
    void Timer::zeroTime()
    {
        if (m_isTimerStarted)
        {
            m_isTimerStarted = false;
            m_isTimerPaused = false;
            m_isTimerZero = true;
            m_pausedTime = {};
            m_startedTime = {}; 
        }
    }

    Timer::steady_clock::duration Timer::getTime()
    {
        if (m_isTimerStarted and !m_isTimerPaused)
        return m_pausedTime + (steady_clock::now().time_since_epoch() - m_startedTime);

        if (m_isTimerStarted and m_isTimerPaused)
        return m_pausedTime;

        if (m_isTimerZero)
        return {};

        return {};
    }

} // namespace Myfcn


