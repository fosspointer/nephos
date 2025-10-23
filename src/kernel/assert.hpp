#pragma once
#define assert(x, msg) if(!(x)) assertThrow(msg)

void assertThrow(const char* message);