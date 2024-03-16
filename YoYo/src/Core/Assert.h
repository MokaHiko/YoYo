#pragma once

#include "Defines.h"
#include <cassert>

namespace yoyo
{
    YAPI void Assert(bool value, const char* message = "");
};

#ifdef Y_DEBUG
#define YASSERT(value, ...) yoyo::Assert(value, ##__VA_ARGS__)
#else
#define YASSERT(value, ...)
#endif

