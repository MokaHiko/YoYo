#include "Log.h"

#include <stdio.h>
#include <stdarg.h>

#include "Platform/Platform.h"

namespace yoyo
{
    void Logger::Log(LOG_LEVEL log_level, const char *message, ...)
    {
		static const char* level_strings[6] =
		{
			"Unknown",
			"INFO",
			"SUCCESS",
			"TRACE",
			"ERROR",
			"WARN",
		};

        char out_message[32000] = {0};

        __builtin_va_list arg_ptr;
        va_start(arg_ptr, message);
        vsnprintf(out_message, 32000, message, arg_ptr);
        va_end(arg_ptr);

        char formatted_message[32000];
        snprintf(formatted_message, 32000, "[%s]: %s \n", level_strings[(int)log_level], out_message);

        Platform::ConsoleWrite(formatted_message, (uint8_t)log_level);
    }
}
