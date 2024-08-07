#include "Time.h"

#include <SDL.h>
namespace yoyo
{
    double Time::m_dt = 0.0;

    ScopedTimer::ScopedTimer(std::function<void(const ScopedTimer&)> finished_callback)
    {
        m_then = SDL_GetTicks();
        m_callback = finished_callback;
    }

    ScopedTimer::~ScopedTimer()
    {
        m_now = SDL_GetTicks();
        delta = m_now - m_then;
        delta /= 1000.0;

        if (m_callback != nullptr)
            m_callback(*this);
    }

	double Time::CurrentTime()
	{
        return SDL_GetTicks();
	}
}