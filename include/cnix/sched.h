#ifndef _CNIX_SCHED_H
#define _CNIX_SCHED_H

union thread{
	struct{
		long stack;
		long canarry1;
	};
	struct{
		long dummy[8192/sizeof(long) -1];
		long canarry2;
	};
};
_Static_assert(sizeof(union thread) == 8192);

static inline union thread* CURTHREAD()
{
    long rsp;
    __asm__ __volatile__("movq %%rsp, %%rax":"=a"(rsp));
    return (union thread*)(rsp & ~8191);
}

#define me CURTHREAD()

#endif
