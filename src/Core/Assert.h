#pragma once

#include "Defines.h"

#include <Core/Log.h>
#include <cassert>

#ifdef Y_DEBUG
#define YASSERT(value, ...)	if(!(value)){YERROR("Assertion Failed: %s", __VA_ARGS__);} \
							assert(value)
#else
#define YASSERT(value, ...)
#endif

namespace yoyo
{
};


