#pragma once

#include "Defines.h"

namespace yoyo
{
    class YAPI Time
    {
    public:
        static double DeltaTime()
        {
            return m_dt;
        };

        static void SetDeltaTime(double dt)
        {
            m_dt = dt;
        };

        static double CurrentTime();
    private:
        static double m_dt;
    };

    // A timer that takes in a function to be called when out object is out of scope
    class YAPI ScopedTimer
    {
    public:
        ScopedTimer(std::function<void(const ScopedTimer&)> finished_callback = nullptr);
        ~ScopedTimer();

        // Delta time in seconds
        double delta = 0;
    private:
        std::function<void(const ScopedTimer&)> m_callback;
        unsigned int m_then, m_now;
    };
};