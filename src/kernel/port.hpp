#pragma once
#include "types.hpp"

namespace port
{
    inline void out(u16 port, u8 value)
    {
        __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(value));
    }

    inline u8 in(u16 port)
    {
        register u8 result;
        __asm__ volatile("inb %1, %0" : "=a"(result) : "dN"(port));
        return result;
    }
}