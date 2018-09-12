#pragma once

// Standard C++ includes
#include <chrono>
#include <iostream>
#include <string>

namespace BoBRobotics {
//------------------------------------------------------------------------
// BoBRobotics::Timer
//------------------------------------------------------------------------
//! A generic timer which can give the current elapsed time
template<typename A = std::milli>
class Timer
{
public:
    //! Create a new Timer with the specified name
    Timer(const std::string &title) : m_Start(std::chrono::high_resolution_clock::now()), m_Title(title)
    {}

    //! Stop the timer and print current elapsed time to terminal
    ~Timer()
    {
        std::cout << m_Title << get() << std::endl;
    }

    //------------------------------------------------------------------------
    // Public API
    //------------------------------------------------------------------------
    //! Get the elapsed time since this object was created
    double get() const
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = now - m_Start;
        return duration.count();
    }

private:
    //------------------------------------------------------------------------
    // Members
    //------------------------------------------------------------------------
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    std::string m_Title;
};

//------------------------------------------------------------------------
// BoBRobotics::TimerAccumulate
//------------------------------------------------------------------------
//! A timer which adds its elapsed time to an accumulator variable on destruction
template<typename A = std::milli>
class TimerAccumulate
{
public:
    TimerAccumulate(double &accumulator) : m_Start(std::chrono::high_resolution_clock::now()), m_Accumulator(accumulator)
    {}

    ~TimerAccumulate()
    {
        m_Accumulator += get();
    }

    //------------------------------------------------------------------------
    // Public API
    //------------------------------------------------------------------------
    //! Get the elapsed time since this object was created
    double get() const
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = now - m_Start;
        return duration.count();
    }

private:
    //------------------------------------------------------------------------
    // Members
    //------------------------------------------------------------------------
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    double &m_Accumulator;
};
} // BoBRobotics
