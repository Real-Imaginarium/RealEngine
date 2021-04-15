#pragma once

#include <stdint.h>


class RegionsAllocator;

class MemoryManager
{
public:
    static bool Initialize();

    static bool Deinitialize();

    static RegionsAllocator generic_allocator;

};

