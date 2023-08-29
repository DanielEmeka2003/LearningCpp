#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace Myfcn
{
    using namespace std::chrono_literals;
    //using namespace std::chrono;

    class Timer
    {
    public:
        using steady_clock = std::chrono::_V2::steady_clock;
    private:
    
        steady_clock::duration m_startedTime{};
        steady_clock::duration m_pausedTime{};
        bool m_isTimerStarted{};
        bool m_isTimerPaused{};
        bool m_isTimerZero{true};

    public:
        Timer() = default;
        Timer(const Timer&) = default;
        Timer(Timer&&) = default;

        Timer& operator=(const Timer&) = default;
        Timer& operator=(Timer&&) = default;

        ~Timer() = default;
        
        /*Starts timer. 
        Once started a second call to the function would do nothing as it would be redundant.*/
        void start();
        /*Restarts timer only if timer has started.*/
        void restart();
        /*Pauses timer only if timer has started.
        Once paused a second call to the function would do nothing as it would be redundant.*/
        void pause();
        /*Unpauses timer only if timer has started and is paused.
        Once unpaused a second call to the function would do nothing as it would be redundant.*/
        void unpause();
        /*Sets the timer to zero, only a call to the start public member function would wake up the timer again.*/
        void zeroTime();
        /*Gets the number of current ticks in the representation/tick period[mostly nanoseconds] specified by system_clock.
        When start() is called    -     gets/starts timer's ticks.
        When restart() is called  -     gets/restarts timer's ticks.
        When pause() is called    -     gets/pauses timer's ticks. 
        When unpause() is called  -     gets/unpauses timer's ticks.
        When zeroTime() is called -     gets/zero's timer's ticks.*/
        Timer::steady_clock::duration getTime();
    };

} // namespace Myfcn


#endif