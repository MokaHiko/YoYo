#pragma once

#include "Defines.h"

namespace yoyo
{
    enum class LOG_LEVEL : uint8_t
    {
        Unknown = 0,
        Info,
        Debug,
        Trace,
        Error,
        Warn
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

#define YFATAL(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::Error, message, ##__VA_ARGS__)
#define YERROR(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::Error , message, ##__VA_ARGS__)
#define YWARN(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::Warn, message, ##__VA_ARGS__)
#define YTRACE(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::Trace, message, ##__VA_ARGS__)
#define YINFO(message, ...) yoyo::Logger::Log(yoyo::LOG_LEVEL::Info, message, ##__VA_ARGS__)