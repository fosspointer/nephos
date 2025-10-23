#pragma once
#include "types.hpp"
#define PIC1 0x20		        /* IO base address for master PIC */
#define PIC2 0xA0		        /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)
#define PIC_EOI	0x20		    /* End-of-interrupt command code */

class PIC final
{
public:
    PIC() = delete;
    static void sendEndOfInterrupt(u8 irq);
    static void remap(i32 first_offset, i32 second_offset);
    static void disable();
    static void setIRQMask(u8 line);
    static void clearIRQMask(u8 line);
    static u16 getIRR();
    static u16 getISR();
private:
    static u16 getIRQRegister(i32 ocw3);
};