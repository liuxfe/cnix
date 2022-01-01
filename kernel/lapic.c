#include <cnix/config.h>
#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/lapic.h>
#include <cnix/traps.h>
#include <cnix/desc.h>

extern void int_lvt_timer();

int32_t* __lapic = 0;

#define rdcoms(_r,_i) 	{ outb(0x70, 0x80| _i); _r = inb(0x71); }
#define COMS_SEC	0x00
#define HZ 60

static uint64_t __init get_tsc_diff()
{
	volatile long s1, s2;
	uint64_t tsc1, tsc2;

	s1 = s2 = 0;
	rdcoms(s1, COMS_SEC);
	do{
		rdcoms(s2, COMS_SEC);
	}while(s1 == s2);
	tsc1 = rdtsc();

	s1 = s2 = 0;
	rdcoms(s1, COMS_SEC);
	do{
		rdcoms(s2, COMS_SEC);
	}while(s1 == s2);
	tsc2 = rdtsc();

	return tsc2 - tsc1;
}

extern void do_sched();
void do_lvt_timer()
{
	lapic_eoi();
	do_sched();
}

void __init setup_lapic(int cpu_id)
{
	int32_t eax, ebx, ecx, edx;

	cpuid(1, 0, eax, ebx, ecx, edx);
	if(!(edx & (1 << 9))){
		printk("NOT support APIC\n");
		while(1){}
	}

	// enable local APIC
	//lapic_write(LAPIC_SVR , lapic_read(LAPIC_SVR) | (1 << 8));

	// Mask ALL LVT
	//lapic_write(LAPIC_LVT_CMCI,  (1<<16));
	//lapic_write(LAPIC_LVT_TIMER, (1<<16));
	//lapic_write(LAPIC_LVT_TM,    (1<<16));
	//lapic_write(LAPIC_LVT_PC,    (1<<16));
	//lapic_write(LAPIC_LVT_LINT0, (1<<16));
	//lapic_write(LAPIC_LVT_LINT1, (1<<16));
	//lapic_write(LAPIC_LVT_ERROR, (1<<16));

	// Clear error status register (requires back-to-back writes).
	lapic_write(LAPIC_ESR, 0);
	lapic_write(LAPIC_ESR, 0);

	// Ack any outstanding interrupts.
	lapic_write(LAPIC_EOI, 0);

	// Is BSP, Send INIT-SIPI-SIPI to AP
	if(cpu_id == 0){
		lapic_write(LAPIC_ICR0,0x000C4500);
		lapic_write(LAPIC_ICR0,0x000C4601);
		lapic_write(LAPIC_ICR0,0x000C4601);
	}

	// Setup Local Timer.
	set_intr_gate(T_LVT_TIMER, (long)int_lvt_timer);
	lapic_write(LAPIC_LVT_TIMER, (1<<17)|T_LVT_TIMER);
	lapic_write(LAPIC_TICR, get_tsc_diff()/HZ/2);
}
