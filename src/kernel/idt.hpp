#pragma once
#include "types.hpp"
#define IDT_MAX_DESCRIPTORS 256

class IDT final
{
public:
    IDT() = delete;
    struct __attribute__((packed)) Entry final
    {
        u16 isrLow;
        u16 kernelCodeSelector;
        u8 reserved;
        u8 attributes;
        u16 isrHigh;
    };

    struct __attribute__((packed)) IDTR final
    {
        u16 limit;
        u32 base;
    };
    
    static void init();
    static void setDescriptor(u8 vector, void* isr, u8 flags);

    __attribute__((aligned(0x10))) static Entry idt[256];
    static IDTR idtr;
    static bool vectors[IDT_MAX_DESCRIPTORS];
};
extern "C" [[noreturn]] void exceptionHandler(u32 vector, u32 code);