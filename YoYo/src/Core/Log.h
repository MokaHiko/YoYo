#pragma once

#include "Defines.h"

namespace yoyo
{
    enum class LOG_LEVEL
    {
        FATAL = 0,
        ERROR,
        WARN,
        INFO,
        DEBUG,
        TRACE
    };

     class YAPI Logger 
    {
    public:
        static void Log(LOG_LEVEL level, const char *message, ...);
    };
}

#ifdef YEBUG
	#define YASSERT(value, ...) mrs::Platform::Assert(value, __VA_ARGS__);
#else
	#define YASSERT(value, ...) 
#endif

#define YFATAL(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::FATAL, message, ##__VA_ARGS__)
#define YERROR(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::ERROR, message, ##__VA_ARGS__)
#define YINFO(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::INFO, message, ##__VA_ARGS__)