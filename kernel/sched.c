#include <cnix/kernel.h>
#include <cnix/sched.h>
#include <cnix/asm.h>
#include <cnix/lapic.h>
#include <cnix/traps.h>
#include <cnix/desc.h>

union thread *th1, *th2;

#define rdcoms(_r,_i) 	{ outb(0x70, 0x80| _i); _r = inb(0x71); }
#define COMS_SEC	0x00
#define HZ 60

uint64_t get_tsc_diff()
{
	volatile long s1, s2;
	uint64_t tsc1, tsc2;

	s1 = s2 = 0;
	rdcoms(s1, COMS_SEC);
	do{
		rdcoms(s2, COMS_SEC);
	}while(s1 == s2);
	tsc1 = rdtsc();

	s1 = s2 = 0;
	rdcoms(s1, COMS_SEC);
	do{
		rdcoms(s2, COMS_SEC);
	}while(s1 == s2);
	tsc2 = rdtsc();

	return tsc2 - tsc1;
}

extern void int_lvt_timer();


// Save TS, XMM, YMM ?
struct ctx_struct{
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t rflags;
	uint64_t retaddr;
};

static long build_ctx(long* p, long func, long arg)
{
	struct ctx_struct *ctx =(struct ctx_struct*)p - 1;
	ctx->retaddr = func;
	ctx->rdi = arg;
	ctx->rflags = 0x10200;
	return (long)ctx;
}

static union thread* create_thread(long func, long arg)
{
	union thread* th;
	long p = alloc_2page();
	if(!p){
		printk("OOM");
		while(1);
	}
	th = (union thread*)__p2v(p);
	th->stack = build_ctx(&(th->canarry2), func, arg);
	return th;
}

extern void __jmp_ctx(long unused, long p);
extern void __switch_ctx(long *from, long to);

void putc_loop(long c)
{
	while(1){
		sti();
		printk("%c", c);
		for(volatile int i=0; i<0x2000000;i++);
	}
}

void sched_init(int how)
{
	th1 = create_thread((long)putc_loop, 'F');
	th2 = create_thread((long)putc_loop, 'S');

	// Setup Local Timer.
	set_intr_gate(T_LVT_TIMER, (long)int_lvt_timer);
	lapic_write(LAPIC_LVT_TIMER, (1<<17)|T_LVT_TIMER);
	lapic_write(LAPIC_TICR, get_tsc_diff()/HZ);

	//__jmp_ctx(0, th1->stack);
}

void do_sched()
{
	union thread *to = (me == th1)? th2:th1;
	__switch_ctx(&(me->stack),to->stack);
}

void do_lvt_timer()
{
	lapic_eoi();
	//printk("D");
}
