#ifndef CNIX_SPINLOCK_H
#define CNIX_SPINLOCK_H

#define SPIN_LOCKED	1
#define SPIN_UNLOCK	0

typedef struct {
	long lock;
} spinlock_t;

static inline void spin_lock(spinlock_t* sl)
{
	__asm__ __volatile__(
		"1:			\n\t"
		"xchgq %%rax, %0	\n\t"
		"cmp   $0, %%rax	\n\t"
		"jne 1b			\n\t"
		:"=m"(sl->lock)
		:"a"(SPIN_LOCKED)
		:"memory"
	);
}

static inline void spin_unlock(spinlock_t* sl)
{
	sl->lock = SPIN_UNLOCK;
}

#endif
