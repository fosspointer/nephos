#include "idt.hpp"
#include "io.hpp"
#include "port.hpp"
#include "pic.hpp"
#define KERNEL_CODE_SELECTOR 0x8

extern "C" [[noreturn]] void exceptionHandler(u32 vector, u32 code)
{
    constexpr u32 exception_map_count = 32;
    static const char* exception_map[exception_map_count] = {
        "Division Error",
        "Debug",
        "Non-maskable Interrupt",
        "Breakpoint",
        "Overflow",
        "Bound Range Exceeded",
        "Invalid Opcode",
        "Device Not Available",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Invalid TSS",
        "Segment Not Present",
        "Stack-Segment Fault",
        "General Protection Fault",
        "Page Fault",
        "Reserved",
        "x87 Floating-Point Exception",
        "Alignment Check",
        "Machine Check",
        "SIMD Floating-Point Exception",
        "Virtualization Exception",
        "Control Protection Exception",
        "Reserved",
        "Hypervisor Injection Exception",
        "VMM Communication Exception",
        "Security Exception",
        "Reserved",
        "Triple Fault",
        "FPU Error Interrupt",
        "Reserved", "Reserved", "Reserved"
    };

    if(vector < exception_map_count)
    {
        IO::characterCursor = 0;
        IO::puts("Exception thrown: ");
        IO::puts(exception_map[vector]);
        if(code != -1)
        {
            IO::puts(" - code ");
            IO::putu(code);
        }
        IO::putln();
    }
    else
    {
        IO::puts("Unknown exception thrown (vector=0x");
        IO::putu(vector, 16);
        IO::puts(", code=0x");
        IO::putu(code, 16);
        IO::puts(")\n");
    }
    __asm__ volatile("cli; hlt");
    __builtin_unreachable();
}

IDT::IDTR IDT::idtr;
bool IDT::vectors[IDT_MAX_DESCRIPTORS];
IDT::Entry IDT::idt[256];

extern void* isrStubTable[];

void isrKeyboardHandler()
{
    port::out(PIC1_COMMAND, PIC_EOI);  // Send EOI to master PIC
    port::out(PIC2_COMMAND, PIC_EOI);
}

void IDT::init()
{
    idtr.base = (usize)&idt[0];
    idtr.limit = (u16)sizeof(Entry) * IDT_MAX_DESCRIPTORS - 1;

    PIC::remap(0x20, 0x28);

    for(u8 vector = 0; vector < 32; ++vector)
    {
        setDescriptor(vector, isrStubTable[vector], 0x8e);
        vectors[vector] = true;
    }

    setDescriptor(0x21, (void*)isrKeyboardHandler, 0x8e);

    __asm__ volatile("lidt %0" : : "m"(idtr));
    port::out(0x21, 0xfd);
    port::out(0xa1, 0xff);
    __asm__ volatile("sti");
}

void IDT::setDescriptor(u8 vector, void* isr, u8 flags)
{
    Entry* descriptor = &idt[vector];
    descriptor->isrLow = (u32)isr & 0xffff;
    descriptor->kernelCodeSelector = KERNEL_CODE_SELECTOR;
    descriptor->attributes = flags;
    descriptor->isrHigh = (u32)isr >> 16;
    descriptor->reserved = 0;
}