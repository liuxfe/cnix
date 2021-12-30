#ifndef _CNIX_MSR_H
#define _CNIX_MSR_H

#define MSR_APIC_BASE	0x01B	// Local APIC Base


#define isBSP()		(rdmsr(MSR_APIC_BASE) & (1<<8))


union __arg {
	int64_t		v;
	struct {
		int32_t l;
		int32_t h;
	};
};

static inline int64_t rdmsr(int index)
{
 	union __arg arg;

	__asm__ __volatile__ (
		"rdmsr \n\t"
		:"=d"(arg.h), "=a"(arg.l)
		:"c"(index)
		:"memory"
	);
	return arg.v;
}

static inline void wrmsr(int32_t index, int64_t value)
{
	union __arg arg;

	arg.v = value;
	__asm__ __volatile__ (
		"wrmsr \n\t"
		:
		:"d"(arg.h), "a"(arg.l), "c"(index)
		:"memory"
	);
}

#endif
