#include <cnix/config.h>
#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/traps.h>
#include <cnix/desc.h>
#include <cnix/lapic.h>

extern void int_keyboard();
void do_keyboard()
{
	lapic_eoi();
	uint8_t c = inb(0x60);
    	printk("kcode:%x", c);
}

void __init setup_kbd()
{
	set_intr_gate(T_KEYBOARD, (long)&int_keyboard);
	ioapic_enable(T_KEYBOARD);
}
