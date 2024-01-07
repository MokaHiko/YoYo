#include "Memory.h"
#include "Platform/Platform.h"

namespace yoyo
{
    YAPI void *YAllocate(size_t size, MemoryTag tag)
    {
        // TODO: Add allocations to memory map 
        return Platform::Allocate(size);
    }
};