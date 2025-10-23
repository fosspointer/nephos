#include "assert.hpp"
#include "io.hpp"

void assertThrow(const char* message)
{
    IO::puts("Assertion failed: ");
    IO::putsln(message);
    __asm("ud2");
}