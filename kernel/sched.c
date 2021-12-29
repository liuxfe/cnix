#include <cnix/kernel.h>
#include <cnix/sched.h>
#include <cnix/asm.h>
#include <cnix/lapic.h>
#include <cnix/traps.h>
#include <cnix/desc.h>

union thread *th1, *th2;
extern void __k();


#define rdcoms(_r,_i) 	{ outb(0x70, 0x80| _i); _r = inb(0x71); }
#define COMS_SEC	0x00
#define HZ 60

uint64_t get_tsc_diff()
{
	long s1, s2;
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

static long create_ctx(long func, long *p)
{
	*(--p) = func;		// ret
	*(--p) = 0x10200;	// popfq
	*(--p) = 0;		// r15
	*(--p) = 0;		// r14
	*(--p) = 0;		// r13
	*(--p) = 0;		// r12
	*(--p) = 0;		// r11
	*(--p) = 0;		// r10
	*(--p) = 0;		// r9
	*(--p) = 0;		// r8
	*(--p) = 0;     	// rbp
	*(--p) = 0;     	// rsi
	*(--p) = 0;   		// rdi
	*(--p) = 0;     	// rdx
	*(--p) = 0;     	// rcx
	*(--p) = 0;     	// rbx
	*(--p) = 0;     	// rax
	return (long)p;
}

static union thread* create_thread(long func)
{
	union thread* th;
	long p = alloc_2page();
	if(!p){
		printk("OOM");
		while(1);
	}
	th = (union thread*)__p2v(p);
	th->stack = create_ctx(func, &(th->canarry2));
	return th;
}

extern void __jmp_ctx(long unused, long p);

void jmp_ctx(long *p)
{
	__asm__ __volatile__(
	"movq %%rax, %%rsp	\n\t"
	"popq %%rax		\n\t"
	"popq %%rbx		\n\t"
	"popq %%rcx		\n\t"
	"popq %%rdx		\n\t"
	"popq %%rdi		\n\t"
	"popq %%rsi		\n\t"
	"popq %%rbp		\n\t"
	"popq %%r8		\n\t"
	"popq %%r9		\n\t"
	"popq %%r10		\n\t"
	"popq %%r11		\n\t"
	"popq %%r12		\n\t"
	"popq %%r13		\n\t"
	"popq %%r14		\n\t"
	"popq %%r15		\n\t"
	"popfq			\n\t"
	"retq			\n\t"
	::"a"(p)
	);
}
void __switch_ctx(long *from, long to);
void switch_ctx(long *from, long to)
{
	printk("SS");

	void __ret();
	// set tss->rsp0
	__asm__ __volatile__ (
	"pushq %%rdx	\n\t"  //return address

	"pushfq			\n\t"
	"pushq %%r15		\n\t"
	"pushq %%r14		\n\t"
	"pushq %%r13		\n\t"
	"pushq %%r12		\n\t"
	"pushq %%r11		\n\t"
	"pushq %%r10		\n\t"
	"pushq %%r9		\n\t"
	"pushq %%r8		\n\t"
	"pushq %%rbp		\n\t"
	"pushq %%rsi		\n\t"
	"pushq %%rdi		\n\t"
	"pushq %%rdx		\n\t"
	"pushq %%rcx		\n\t"
	"pushq %%rbx		\n\t"
	"pushq %%rax		\n\t"
	"mov %%rsp, (%%rbx)	\n\t" // save old rsp
	"mov %%rcx, %%rsp 	\n\t" // load new rsp
	"popq %%rax		\n\t"
	"popq %%rbx		\n\t"
	"popq %%rcx		\n\t"
	"popq %%rdx		\n\t"
	"popq %%rdi		\n\t"
	"popq %%rsi		\n\t"
	"popq %%rbp		\n\t"
	"popq %%r8		\n\t"
	"popq %%r9		\n\t"
	"popq %%r10		\n\t"
	"popq %%r11		\n\t"
	"popq %%r12		\n\t"
	"popq %%r13		\n\t"
	"popq %%r14		\n\t"
	"popq %%r15		\n\t"
	"popfq			\n\t"
	"retq			\n\t"
	"__ret:		  	\n\t"
	::"b"(from),"c"(to), "d"(&__ret)
    );
}

void loop1()
{
	while(1){
		sti();
		printk("A");
		for(volatile int i=0; i<0x2000000;i++);
	}
}

void loop2()
{
	while(1){
		sti();
		printk("B");
		for(volatile int i=0; i<0x1000000;i++);
	}
}

void sched_init(int how)
{
	th1 = create_thread((long)loop1);
	th2 = create_thread((long)loop2);

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
	//printk("D");
	lapic_eoi();
}
