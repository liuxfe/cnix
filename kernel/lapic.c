#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/lapic.h>

int32_t* __lapic = 0;

void lapic_init(int cpu_id)
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
}
