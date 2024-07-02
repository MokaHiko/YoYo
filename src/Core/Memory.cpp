#include "Memory.h"

#include "Platform/Platform.h"
#include "Core/Assert.h"

static std::unordered_map<void*, yoyo::MemoryAllocation> s_memory_map = {};

namespace yoyo
{
    YAPI void *YAllocate(size_t size, MemoryTag tag)
    {
        void* data = Platform::Allocate(size);

        YASSERT(data, "Failed to allocate memory!");
        s_memory_map[data] = {size, tag};

        return data;
    }

	YAPI void YFree(void* data)
	{
        YASSERT(data, "Attempting to free unallocated memory!");

        auto it = s_memory_map.find(data);
        if(it != s_memory_map.end())
        {
            //it->second.size
            free(data);
        }
        else
        {
            YASSERT(0, "Attempting to free memory not allocated by engine!");
        }
	}

    const std::unordered_map<void*, MemoryAllocation>& GetMemoryMap()
    {
        return s_memory_map;
    }

    void AddToMemoryMap(void* data, size_t size, MemoryTag tag)
    {
        YASSERT(data, "Failed to allocate memory!");
        s_memory_map[data] = {size, tag};
    }

    void RemoveFromMemoryMap(void* data) noexcept
    {
        YASSERT(data, "Cannot remove unallocate memory from memory map!");
    
        auto it = s_memory_map.find(data);
        if (it != s_memory_map.end())
        {
            s_memory_map.erase(it);
        }
    }
};