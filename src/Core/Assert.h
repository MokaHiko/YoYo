#pragma once

#include <Core/Log.h>

#ifdef Y_DEBUG

#ifdef Y_PLATFORM_WIN32
#include <cassert>
#define YASSERT(value, ...)	if(!(value)){YERROR("Assertion Failed: %s", __VA_ARGS__);} \
							assert(value)
#endif

#ifdef Y_PLATFORM_APPLE
#define YASSERT(value, ...)
#endif

#elif
#define YASSERT(value, ...)

#endif

