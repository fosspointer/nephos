#pragma once
#include "types.hpp"

class Memory final
{
public:
    Memory() = delete;
    static usize allocationAddress;
    static void* allocate(usize bytes);
    static void free(void* address);
};
