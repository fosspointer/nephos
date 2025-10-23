#include "pic.hpp"
#include "port.hpp"
#include "io.hpp"
#define ICW1_ICW4 0x01		    /* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04	    /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08		    /* Level triggered (edge) mode */
#define ICW1_INIT 0x10		    /* Initialization - required! */

#define ICW4_8086 0x01		    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02		    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	/* Buffered mode/master */
#define ICW4_SFNM 0x10		    /* Special fully nested (not) */

#define PIC_READ_IRR 0x0a       /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b       /* OCW3 irq service next CMD read */

void sendEndOfInterrupt(u8 irq)
{
    if(irq >= 8)
        port::out(PIC2_COMMAND, PIC_EOI);
}

void PIC::remap(i32 first_offset, i32 second_offset)
{
	port::out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // starts the initialization sequence (in cascade mode)
	IO::wait();
	port::out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	IO::wait();
	port::out(PIC1_DATA, first_offset);             // ICW2: Master PIC vector offset
	IO::wait();
	port::out(PIC2_DATA, second_offset);            // ICW2: Slave PIC vector offset
	IO::wait();
	port::out(PIC1_DATA, 4);                        // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	IO::wait();
	port::out(PIC2_DATA, 2);                        // ICW3: tell Slave PIC its cascade identity (0000 0010)
	IO::wait();
	
	port::out(PIC1_DATA, ICW4_8086);                // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	IO::wait();
	port::out(PIC2_DATA, ICW4_8086);
	IO::wait();

	// Unmask both PICs.
	port::out(PIC1_DATA, 0);
	port::out(PIC2_DATA, 0);
}

void PIC::disable()
{
    port::out(PIC1_DATA, 0xff);
    port::out(PIC2_DATA, 0xff);
}

void PIC::setIRQMask(u8 line)
{
    u16 port;
    u8 value;

    if(line < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        line -= 8;
    }
    value = port::in(port) | (1 << line);
    port::out(port, value); 
}

void PIC::clearIRQMask(u8 line)
{
    u16 port;
    u8 value;

    if(line < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        line -= 8;
    }
    value = port::in(port) & ~(1 << line);
    port::out(port, value);   
}

u16 PIC::getIRR()
{
    return getIRQRegister(PIC_READ_IRR);
}

u16 PIC::getISR()
{
    return getIRQRegister(PIC_READ_ISR);
}

u16 PIC::getIRQRegister(i32 ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    port::out(PIC1_COMMAND, ocw3);
    port::out(PIC2_COMMAND, ocw3);
    return (port::in(PIC2_COMMAND) << 8) | port::in(PIC1_COMMAND);
}
