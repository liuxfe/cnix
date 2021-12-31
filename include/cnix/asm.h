#ifndef _CNIX_ASM_H
#define _CNIX_ASM_H

#define cli()           __asm__ ("cli \n\t");
#define sti()           __asm__ ("sti \n\t");
#define hlt()           __asm__ ("hlt \n\t");

#define xchg(_p, _v) 			\
	__asm__ __volatile__(		\
	"xchgq %%rax, %0 \n\t"		\
	:"=m"(_p)			\
	:"a"(_v)			\
	:"memory"			\
	);

#define lcr3(_v)                        \
	__asm__ __volatile__(           \
		"mov %%rax, %%cr3 \n\t" \
		::"a"(_v)               \
	);

static inline unsigned char inb(short p)
{
	char r;
	__asm__ __volatile__ (
		"inb  %%dx, %%al \n\t"
		"mfence          \n\t"
		:"=a"(r) :"d"(p)
		:"memory"
	);
	return r;
}
#define outb(_p,_v)                     \
	__asm__ __volatile__ (          \
		"outb  %%al, %%dx \n\t" \
		"mfence           \n\t" \
		::"a"(_v),"d"(_p)       \
	);
#define inw(_r,_p)                      \
	__asm__ __volatile__ (          \
		"inw  %%dx, %%ax \n\t"  \
		"mfence          \n\t"  \
		:"=a"(_r)               \
		:"d"(_p)                \
	);
#define inl(_r,_p)                      \
	__asm__ __volatile__ (          \
		"inl  %%dx, %%eax \n\t" \
		"mfence           \n\t" \
		:"=a"(_r)               \
		:"d"(_p)                \
	);

#define outw(_p,_v)                     \
	__asm__ __volatile__ (          \
		"outw  %%ax, %%dx \n\t" \
		"mfence           \n\t" \
		::"a"(_v),"d"(_p)       \
	);
#define outl(_p,_v)                     \
	__asm__ __volatile__ (          \
		"outl  %%eax, %%dx \n\t"\
		"mfence            \n\t"\
		::"a"(_v),"d"(_p)       \
	);
#define insb(_p,_b,_c)                  \
	__asm__ __volatile__ (          \
		"cld        \n\t"       \
		"rep insb   \n\t"       \
		"mfence     \n\t"       \
		:                       \
		:"d"(_p),"D"(_b),"c"(_c)\
	);
#define insw(_p,_b,_c)                  \
	__asm__ __volatile__ (          \
		"cld        \n\t"       \
		"rep insw   \n\t"       \
		:                       \
		:"d"(_p),"D"(_b),"c"(_c)\
		:"memory"		\
	);
#define insl(_p,_b,_c)                  \
	__asm__ __volatile__ (          \
		"cld        \n\t"       \
		"rep insl   \n\t"       \
		"mfence     \n\t"       \
		:                       \
		:"d"(_p),"D"(_b),"c"(_c>>2)\
	);
#define outsb(_p,_b,_c)                 \
	__asm__ __volatile__ (          \
		"cld        \n\t"       \
		"rep outsb  \n\t"       \
		"mfence     \n\t"       \
		:                       \
		:"d"(_p),"S"(_b),"c"(_c)\
	);
#define outsw(_p,_b,_c)                 \
	__asm__ __volatile__ (          \
		"cld        \n\t"       \
		"rep outsw  \n\t"       \
		"mfence     \n\t"       \
		:                       \
		:"d"(_p),"S"(_b),"c"(_c>>1)\
	);
#define outsl(_p,_b,_c)                 \
	__asm__ __volatile__ (          \
		"cld        \n\t"       \
		"rep outsl  \n\t"       \
		"mfence     \n\t"       \
		:                       \
		:"d"(_p),"S"(_b),"c"(_c>>2)\
	);

#define cpuid(_l,_s,_a,_b,_c,_d)        \
	__asm__ __volatile__ (          \
		"cpuid \n\t"            \
		:"=a"(_a),"=b"(_b),     \
		 "=c"(_c),"=d"(_d)      \
		:"0"(_l),"2"(_s)        \
	);

static inline int64_t rdtsc()
{
	union {
		int64_t		v;
		struct {
			int32_t	l;
			int32_t	h;
		};
	} arg;

	 __asm__ __volatile__ (
		"rdtsc \n\t"
		:"=d"(arg.h), "=a"(arg.l)
		:
		:"memory"
	);
	return arg.v;
}

#endif
