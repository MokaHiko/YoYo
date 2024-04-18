#pragma once

#include "Defines.h"
#include "Core/Assert.h"

namespace yoyo
{
    enum class MemoryTag
    {
        Uknown,
        Renderer,
        String,

        Resource,

        Game,

        SmartPtr,

        MaximumValue,
    };

    static const char* MemoryTagStrings[]
    {
        "Uknown",
        "Renderer",
        "String",

        "Resource",

        "Game",

        "SmartPtr",
        "MaximumValue",
    };

    struct MemoryAllocation
    {
        uint64_t size;
        MemoryTag tag;
    };

    YAPI void * YAllocate(size_t size, MemoryTag tag = MemoryTag::Uknown);
    YAPI void YFree(void* data);

    YAPI const std::unordered_map<void*, MemoryAllocation>& GetMemoryMap();

    // Used to keep track of memory not explicitly created using Yallocate or Yreallocate
    YAPI void AddToMemoryMap(void* data, size_t size, MemoryTag tag);
    YAPI void RemoveFromMemoryMap(void* data) noexcept;
}

#ifdef Y_DEBUG

#ifdef Y_COMPILER_MSVC
#define YNEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define YDELETE delete
#else
#define YNEW new
#define YDELETE delete
#endif

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
YAPI constexpr Ref<T> CreateRef(Args &&...args)
{
    std::shared_ptr<T> data(new T{ std::forward<Args>(args)... }, [](T* ptr) {
        yoyo::RemoveFromMemoryMap(ptr);
        delete ptr;
    });

	YASSERT(data, "Failed to allocate memory!");
    yoyo::AddToMemoryMap((void*)data.get(), sizeof(T), yoyo::MemoryTag::SmartPtr);

    return data;
}

template <typename T>
using Scope = std::unique_ptr<T, void(*)(T*)>;
template <typename T, typename... Args>
YAPI constexpr Scope<T> CreateScope(Args &&...args)
{
    std::unique_ptr<T, void(*)(T*)> data(new T{ std::forward<Args>(args)... }, [](T* ptr) {
        yoyo::RemoveFromMemoryMap(ptr);
        delete ptr;
    });

	YASSERT(data, "Failed to allocate memory!");
    yoyo::AddToMemoryMap((void*)data.get(), sizeof(T), yoyo::MemoryTag::SmartPtr);

    return data;
}

template <typename T>
using WeakRef = std::weak_ptr<T>;

#else

#define YNEW new
#define YDELETE delete

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args &&...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
YAPI constexpr Ref<T> CreateRef(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using WeakRef = std::weak_ptr<T>;
#endif