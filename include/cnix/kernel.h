#ifndef _CNIX_KERNEL_H
#define _CNIX_KERNEL_H

typedef signed char             int8_t;
typedef unsigned char           uint8_t;
typedef signed short            int16_t;
typedef unsigned short          uint16_t;
typedef signed int              int32_t;
typedef unsigned int            uint32_t;
typedef signed long long        int64_t;
typedef unsigned long long      uint64_t;

#define __init __attribute__((section (".init")))

// stdbool
#define bool	_Bool
#define true	1
#define false	0

// NULL
#define NULL	((void*)0)

#define PHYOFF		0xFFFF800000000000

#define __p2v(a)	((a) + PHYOFF)
#define __v2p(a)	((a) - PHYOFF)


#define memcpy(_d,_s,_c)                \
	__asm__ __volatile__ (          \
		"rep movsb \n\t"        \
		::"D"(_d), "S"(_s), "c"(_c) :"memory");

#define memset(_d,_v,_c)                \
	__asm__ __volatile__ (          \
		"rep stosb \n\t"        \
		::"D"(_d), "a"(_v), "c"(_c) :"memory");

#define memzero(_d,_c)  memset(_d, 0, _c)

extern void printk(char* fmt, ...);
extern void ioapic_enable(int irq);
extern void ioapic_eoi(int irq);
extern void lapic_eoi();

extern long alloc_page();
extern long alloc_2page();

extern int NR_CPUS;

#endif
