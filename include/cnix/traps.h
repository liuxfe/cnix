#ifndef _CNIX_TRAPS_H
#define _CNIX_TRAPS_H

union IDTdesc64 {
    struct {
	int64_t low;
	int64_t high;
    };
    struct {
	int   offset0_15 : 16;
	int   selector   : 16;
	int   ist        : 3;
	int   resvered1  : 5;
	int   attr       : 8;
	int   offset16_31: 16;
	int   offset32_63: 32;
	int   resvered2  : 32;
    };
};
extern union IDTdesc64 IDTtable[256];

extern inline void _set_idt64(long nr, long addr, long attr)
{
	union IDTdesc64 * idt = IDTtable + nr;

	idt->high = 0x00000000ffff8000;
	idt->low  = 0x00008E0000080000;

	idt->offset0_15 = addr & 0xffff;
	idt->offset16_31 = (addr >> 16) & 0xffff;
	idt->attr = attr;
}

#define set_trap_gate(nr, addr) _set_idt64(nr, addr, 0x8F)
#define set_intr_gate(nr, addr) _set_idt64(nr, addr, 0x8E)
#define set_call_gate(nr, addr) _set_idt64(nr, addr, 0xEF)

struct trapregs {
	unsigned long ds;
	unsigned long es;
	unsigned long rax;
	unsigned long rbx;
	unsigned long rcx;
	unsigned long rdx;
	unsigned long rdi;
	unsigned long rsi;
	unsigned long rbp;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	unsigned long r13;
	unsigned long r14;
	unsigned long r15;
	unsigned long rip;
	unsigned long cs;
	unsigned long rflags;
	unsigned long rsp;
	unsigned long ss;
};

#endif
