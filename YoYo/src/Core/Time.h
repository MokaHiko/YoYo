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
    private:
        static double m_dt;
    };
};