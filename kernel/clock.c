#include <cnix/kernel.h>
#include <cnix/traps.h>
#include <cnix/asm.h>
#include <cnix/desc.h>

#define HZ	60
#define LATCH	(1193180/HZ)

unsigned long startup_click = 0;

extern void trap_clock_intr();
void clock_init()
{
    outb(0x43, 0x36);
    outb(0x40, LATCH && 0xff);
    outb(0x40, LATCH >> 8);
    //outb(0x21, inb(0x21) & ~1);
    set_intr_gate(T_IOAPIC + T_CLOCK, (long)trap_clock_intr);
    ioapic_enable(T_CLOCK);
}

extern void do_sched();
void do_clock_intr(struct trapregs* regs)
{
        ++startup_click;

        lapic_eoi();
        if((startup_click % 100) == 0){
        do_sched();
    }
}
