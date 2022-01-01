#ifndef _CNIX_SCHED_H
#define _CNIX_SCHED_H

struct TSS_struct{
	uint32_t	reserved1;
	uint64_t	rsp0;
	uint64_t	rsp1;
	uint64_t	rsp2;
	uint64_t	reserved2;
	uint64_t	ist1;
	uint64_t	ist2;
	uint64_t	ist3;
	uint64_t	ist4;
	uint64_t	ist5;
	uint64_t	ist6;
	uint64_t	ist7;
	uint64_t	reserved3;
	uint16_t	reserved4;
	uint16_t	io_map_base_address;
} __attribute__((packed)) ;
_Static_assert(sizeof(struct TSS_struct) == 104, "");

union thread{
	struct{
		long stack;
		long cpu_id;
		union thread* next; // ready list
		long canarry1;
	};
	struct{
		long dummy[8192/sizeof(long) -1];
		long canarry2;
	};
};
_Static_assert(sizeof(union thread) == 8192, "");

struct cpu_struct{
	struct TSS_struct tss;
	union thread *idle;
	union thread *ready;
};

extern struct cpu_struct cpu_tab[NR_CPU_MAX];

static inline union thread* CURTHREAD()
{
	uint64_t rsp;
	__asm__ __volatile__("movq %%rsp, %%rax":"=a"(rsp));
	return (union thread*)(rsp & ~8191ULL);
}

#define me CURTHREAD()

extern union thread* kthread(long func, long arg, long cpu_id);

#endif
