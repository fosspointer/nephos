#include "printf.hpp"
#include "types.hpp"
#include "io.hpp"
#include "assert.hpp"

void printf(const char* str, ...)
{
    usize argument = reinterpret_cast<usize>(&str) + sizeof(const char*);

    char character;
    while((character = *(str++)))
    {
        if(character == '%')
        {
            switch(*(str++))
            {
            case '%':
                IO::putc('%');
                break;
            case 'd':
            case 'i':
            {
                i32 value = *reinterpret_cast<i32*>(argument);
                argument += sizeof(i32);
                IO::puti(value);
                break;
            }
            case 'u':
            {
                u32 value = *reinterpret_cast<u32*>(argument);
                argument += sizeof(u32);
                IO::putu(value);
                break;
            }
            case 'x':
            case 'X':
            {
                u32 value = *reinterpret_cast<u32*>(argument);
                argument += sizeof(u32);
                IO::putu(value, 16);
                break;
            }
            case 'o':
            {
                u32 value = *reinterpret_cast<u32*>(argument);
                argument += sizeof(u32);
                IO::putu(value, 8);
                break;
            }
            case 'c':
            {
                u8 value = *reinterpret_cast<u8*>(argument);
                argument += sizeof(u8);
                IO::putc(value);
                break;
            }
            case 's':
            {
                const char* value = *reinterpret_cast<const char**>(argument);
                argument += sizeof(const char*);
                IO::puts(value);
                break;
            }
            case 'p':
            {
                const void* value = *reinterpret_cast<const void**>(argument);
                argument += sizeof(const void*);
                IO::putu(reinterpret_cast<usize>(value));
                break;
            }
            case 'f':
            {
                f32 value = *reinterpret_cast<f32*>(argument);
                argument += sizeof(f32);

                IO::putf(value);
                break;
            }
            default: assert(false, "invalid format specifier");
            }
            continue;
        }
        IO::putc(character);
    }
}