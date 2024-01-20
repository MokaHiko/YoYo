#pragma once

namespace yoyo
{
    class Time
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
    class Timer
    {
    public:
        Timer(std::function<void(const Timer&)> finished_callback = nullptr);
        ~Timer();

        // Delta time in seconds
        double delta = 0;
    private:
        std::function<void(const Timer&)> m_callback;
        unsigned int m_then, m_now;
    };
};