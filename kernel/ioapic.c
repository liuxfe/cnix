#include <cnix/config.h>
#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/traps.h>

#define IOAPIC_ID	0x00
#define IOAPIC_VER	0x01
#define IOAPIC_RTE0	0x10

struct ioapic{
	volatile uint32_t  reg;
	uint32_t  pad0;
	uint64_t  pad1;
	union{
		volatile uint32_t data32;
		volatile uint64_t data64;
	};
	uint64_t  pad2[5];
	volatile uint32_t  EOI;
} *ioapic = NULL;

static inline int32_t ioapic_read32(int reg)
{
	ioapic->reg = reg;
	return ioapic->data32;
}

static inline int64_t ioapic_read64(int reg)
{
	ioapic->reg = reg;
	return ioapic->data64;
}

static inline void iopaic_write(int reg, int64_t data)
{
	ioapic->reg = reg;
	ioapic->data64 = data;
}

static inline void iopaic_write32(int reg, int32_t data)
{
	ioapic->reg = reg;
	ioapic->data32 = data;
}

void __init setup_ioapic()
{
	if(!ioapic){
		printk("IOAPIC Not Found!");
	}

	int ver = ioapic_read32(IOAPIC_VER);
	//printk("IOAPIC-ID:%x\nIOAPIC-VERSION:%x\n",
	//	ioapic_read32(IOAPIC_ID), ver);
	int maxintr = (ver >> 16) & 0xff;
	//printk("maxintr=%d\n", maxintr);

	for(int i=0; i<= maxintr; i++){
		iopaic_write32(IOAPIC_RTE0 + 2*i, (1<<16) | (i + 0x20));
	}

	outb(0x21,0xff); outb(0xA1,0xff); // Mask all 8259A.

	__asm__ __volatile__("":::"memory");
}

void ioapic_enable(int irq)
{
	iopaic_write32(IOAPIC_RTE0 + 2*(irq - T_IOAPIC),  irq);
}

void ioapic_eoi(int irq)
{
	ioapic->EOI = irq;
}
