#ifndef _CNIX_TRAPS_H
#define _CNIX_TRAPS_H

#define T_DIVIDE	0	/* divide error				*/
#define T_DEBUG		1	/* debug exception			*/
#define T_NMI		2	/* non-maskable interrupt		*/
#define T_BRKPT		3	/* breakpoint				*/
#define T_OFLOW		4	/* overflow				*/
#define T_BOUND		5	/* bounds check				*/
#define T_ILLOP		6	/* illegal opcode 			*/
#define T_DEVICE	7	/* device not available			*/
#define T_DBLFLT	8	/* double fault				*/
#define T_COPROC	9	/* reserved (not used since 486)	*/
#define T_TSS		10	/* invalid task switch segment		*/
#define T_SEGNP		11	/* segment not present			*/
#define T_STACK		12	/* stack exception			*/
#define T_GPFLT		13	/* general protection fault		*/
#define T_PGFLT		14	/* page fault				*/
#define T_RES		15	/* reserved				*/
#define T_FPERR		16	/* floating point error			*/
#define T_ALIGN		17	/* aligment check			*/
#define T_MCHK		18	/* machine check			*/
#define T_SIMDERR	19	/* SIMD floating point error		*/

#define T_IOAPIC	0x20
#  define T_8259A	0x20	// Unused.
#  define T_KEYBOARD	0x21
#  define T_CLOCK	0x22
#  define T_IDE0	0x2E

#define T_LocalAPIC	0x38
#  define T_LVT_CMCI  	0x38
#  define T_LVT_TIMER	0x39	// LVT timer
#  define T_LVT_TM	0x3A	// LVT Thermal Monitor
#  define T_LVT_PC	0x3B	// LVT Performance Counter
#  define T_LVT_LINT0	0x3C
#  define T_LVT_LINT1	0x3D
#  define T_LVT_ERROR	0x3E

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
