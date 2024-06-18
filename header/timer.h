#ifndef TIMER_H
#define TIMER_H

#pragma once

#include <chrono>

namespace misc
{
    class Timer
    {
    private:
    
        std::chrono::steady_clock::duration m_startedTime{};
        std::chrono::steady_clock::duration m_pausedTime{};
        //is a covariant to m_isTimerZero
        bool m_isTimerStarted{};
        bool m_isTimerPaused{};
        //is a covariant to m_isTimerStarted
        bool m_isTimerZero{true};

    public:
        constexpr Timer() noexcept = default;
        
        Timer(const Timer&) noexcept = default;
        Timer(Timer&&) noexcept = default;

        Timer& operator=(const Timer&) noexcept = default;
        Timer& operator=(Timer&&) noexcept = default;

        constexpr ~Timer() noexcept = default;
        
        /*Starts timer. Once started subsequent calls to the function would do nothing as it would be redundant.*/
        void start() noexcept;
        /*Restarts timer only if timer has started.*/
        void restart() noexcept;
        /*Pauses timer only if timer has started.
        Once paused subsequent calls to the function would do nothing as it would be redundant.*/
        void pause() noexcept;
        /*Unpauses timer only if timer has started and is paused.
        Once unpaused subsequent calls to the function would do nothing as it would be redundant.*/
        void unpause() noexcept;
        /*Sets the Timer to zero, only a call to the start public member function would wake up the Timer again.
        Once the timer is zeroed, subsequent calls to the function would do nothing as it would be redundant.*/
        void zeroTime() noexcept;

        /*Returns a boolean value true only if started and false otherwise.*/
        constexpr bool is_started() const noexcept;
        /*Returns a boolean value true only if timer is started and paused and false otherwise.*/
        constexpr bool is_paused() const noexcept;
        /*Returns a boolean value true only if timer is zeroed and false otherwise.*/
        constexpr bool is_zeroed() const noexcept;
        /*Gets the Timer's current ticks in the duration specified by steady_clock[std::chrono::nanoseconds].*/
        constexpr std::chrono::steady_clock::duration getTicks() const noexcept;
    };

    constexpr bool Timer::is_started() const noexcept
    { return m_isTimerStarted; }

    constexpr bool Timer::is_paused() const noexcept
    { return m_isTimerPaused; }

    constexpr bool Timer::is_zeroed() const noexcept
    { return m_isTimerZero; }

    constexpr std::chrono::steady_clock::duration Timer::getTicks() const noexcept
    {
        if (m_isTimerStarted and !m_isTimerPaused)
        return m_pausedTime + (std::chrono::steady_clock::now().time_since_epoch() - m_startedTime);

        else if (m_isTimerStarted and m_isTimerPaused)
        return m_pausedTime;

        else if (m_isTimerZero)
        return {};

        return {};
    }
    
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


#endif