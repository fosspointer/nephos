#include "memory.hpp"

usize Memory::allocationAddress = 0x01000000;

void* Memory::allocate(usize bytes)
{
    auto result = reinterpret_cast<void*>(Memory::allocationAddress);
    Memory::allocationAddress += bytes;
    return result;
}

void Memory::free(void* address) {}