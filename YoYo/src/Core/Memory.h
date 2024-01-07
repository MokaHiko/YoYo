#pragma once

#include "Defines.h"

#ifdef Y_DEBUG

#ifdef Y_COMPILER_MSVC
#define Y_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define Y_NEW new
#endif

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
namespace yoyo
{
    enum class MemoryTag
    {
        UKNOWN,
        RENDERER,
        STRING,
    };

    YAPI void *
    YAllocate(size_t size, MemoryTag tag);
}

#else
#define Y_NEW new

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
#endif