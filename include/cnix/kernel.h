#ifndef _CNIX_KERNEL_H
#define _CNIX_KERNEL_H

#define PHYOFF		0xFFFF800000000000

#define __p2v(a)	((a) + PHYOFF)
#define __v2p(a)	((a) - PHYOFF)

static inline char inb(short p)
{
	char r;
	__asm__ __volatile__ (
		"inb  %%dx, %%al \n\t" :"=a"(r) :"d"(p)
	);
	return r;
}

#define outb(_p,_v)                     \
	__asm__ __volatile__ (          \
		"outb  %%al, %%dx \n\t" \
		"mfence           \n\t" \
		::"a"(_v),"d"(_p)       \
	);

#define memcpy(_d,_s,_c)                \
	__asm__ __volatile__ (          \
		"rep movsb \n\t"        \
		::"D"(_d), "S"(_s), "c"(_c) :"memory");

#define memset(_d,_v,_c)                \
	__asm__ __volatile__ (          \
		"rep stosb \n\t"        \
		::"D"(_d), "a"(_v), "c"(_c) :"memory");

#define memzero(_d,_c)  memset(_d, 0, _c)

#endif
