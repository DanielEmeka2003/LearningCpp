#include "timer.h"

namespace Myfcn
{
    void Timer::start() noexcept
    {
        if (!m_isTimerStarted)//can be replaced with (m_isTimerZero)
        {
            m_isTimerStarted = true;
            m_isTimerPaused = false;
            m_isTimerZero = false;
            m_pausedTime = {};
            m_startedTime = std::chrono::steady_clock::now().time_since_epoch();
        }
    }

    void Timer::restart() noexcept
    {
        if (m_isTimerStarted)//can be replaced with (!m_isTimerZero)
        {
            m_isTimerStarted = true;
            m_isTimerPaused = false;
            m_isTimerZero = false;
            m_pausedTime = {};
            m_startedTime = std::chrono::steady_clock::now().time_since_epoch();    
        }
    }

    void Timer::pause() noexcept
    {
        if (m_isTimerStarted and !m_isTimerPaused)//can be replaced with (!m_isTimerZero and !m_isTimerPaused)
        {  
            m_isTimerStarted = true;
            m_isTimerPaused = true;
            m_isTimerZero = false;
            m_pausedTime = m_pausedTime + (std::chrono::steady_clock::now().time_since_epoch() - m_startedTime);
            m_startedTime = {};
        }
    }

    void Timer::unpause() noexcept
    {
        if (m_isTimerStarted and m_isTimerPaused)//can be replaced with (!m_isTimerZero and m_isTimerPaused)
        {
            m_isTimerPaused = false;
            m_isTimerStarted = true;
            m_isTimerZero = false;
            /* PausedTime remains with its value. */
            m_startedTime = std::chrono::steady_clock::now().time_since_epoch();
        }
    }
    
    void Timer::zeroTime() noexcept
    {
        if (m_isTimerStarted)//can be replaced with (!m_isTimerZero)
        {
            m_isTimerStarted = false;
            m_isTimerPaused = false;
            m_isTimerZero = true;
            m_pausedTime = {};
            m_startedTime = {}; 
        }
    }

    

} // namespace Myfcn
