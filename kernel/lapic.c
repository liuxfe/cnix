#include <cnix/kernel.h>
#include <cnix/asm.h>

#define MSR_APIC_BASE	0x01B	// Local APIC Base

#define LAPIC_ID 	0x020
#define LAPIC_VER	0x030
#define LAPIC_TPR	0x080
#define LAPIC_APR	0x090
#define LAPIC_PPR	0x0A0
#define LAPIC_EOI	0x0B0
#define LAPIC_RRD	0x0C0
#define LAPIC_LDR	0x0D0
#define LAPIC_DFR	0x0E0
#define LAPIC_SVR	0x0F0
#define LAPIC_ISR	0x100
#define LAPIC_IMR	0x180
#define LAPIC_IRR	0x200
#define LAPIC_ESR	0x280
#define LAPIC_LVT_CMCI  0x290
#define LAPIC_ICR0 	0x300
#  define INIT     0x00000500   // INIT/RESET
#  define STARTUP  0x00000600   // Startup IPI
#  define DELIVS   0x00001000   // Delivery status
#  define ASSERT   0x00004000   // Assert interrupt (vs deassert)
#  define DEASSERT 0x00000000
#  define LEVEL    0x00008000   // Level triggered
#  define BCAST    0x00080000   // Send to all APICs, including self.
#  define BUSY     0x00001000
#  define FIXED    0x00000000
#define LAPIC_ICR1      0x310
#define LAPIC_LVT_TIMER 0x320
#define LAPIC_LVT_TM    0x330	// LVT Thermal Moniter register
#define LAPIC_LVT_PC    0x340   // LVT Performance Monitoring Counter register
#define LAPIC_LVT_LINT0 0x350
#define LAPIC_LVT_LINT1 0x360
#define LAPIC_LVT_ERROR 0x370
#define LAPIC_TICR      0x380   // Timer initial count register
#define LAPIC_TCCR      0x390   // Timer current count register
#define LAPIC_IDCR      0x3E0   // Timer divide config register

int32_t* lapicbase = 0;

static inline int32_t lapic_read(long offset)
{
	return lapicbase[offset>>2];
}

static inline void lapic_write(long offset, int32_t value)
{
	lapicbase[offset>>2] = value;
	__asm__ __volatile__("":::"memory");
}

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

	// Setup Local Timer.
	//lapic_set_timer(0x30000000, lapic_timer_boot_traphandle);
}

void lapic_eoi()
{
	lapic_write(LAPIC_EOI,0);
}
