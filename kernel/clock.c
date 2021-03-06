#include <cnix/config.h>
#include <cnix/kernel.h>
#include <cnix/traps.h>
#include <cnix/asm.h>
#include <cnix/desc.h>
#include <cnix/lapic.h>

#define HZ	60
#define LATCH	(1193180/HZ)

unsigned long startup_click = 0;

extern void int_clock();
void __init clock_init()
{
	outb(0x43, 0x36);
	outb(0x40, LATCH && 0xff);
	outb(0x40, LATCH >> 8);
	//outb(0x21, inb(0x21) & ~1);
	set_intr_gate(T_CLOCK, (long)int_clock);
	ioapic_enable(T_CLOCK);
}

void do_clock()
{
	++startup_click;
	lapic_eoi();
}
