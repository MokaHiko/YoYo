#pragma once

#include <Core/Log.h>

#ifdef Y_DEBUG

#ifdef Y_PLATFORM_WINDOWS
#include <cassert>
#define YASSERT(value, ...)	if(!(value)){YERROR("Assertion Failed: %s", __VA_ARGS__);} \
							assert(value)
#endif

#ifdef Y_PLATFORM_APPLE
#define YASSERT(value, ...)
#endif

#endif

namespace yoyo
{
};


