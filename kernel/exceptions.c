#include <cnix/kernel.h>
#include <cnix/traps.h>
#include <cnix/asm.h>
#include <cnix/sched.h>

static void _die(long rsp, long err_code, char* info)
{
	struct trapregs* r = (struct trapregs*)rsp;

	printk("\nCPU %d: %s , err=%0#18x\n", me->cpu_id, info, err_code);
	printk("ss=%0#18x,rsp=%0#18x\n", r->ss, r->rsp);
	printk("rflags=%0#18x\n", r->rflags);
	printk("cs =%0#18x,rip=%0#18x\n", r->cs, r->rip);
	printk("rax=%0#18x,rbx=%0#18x,rcx=%0#18x\n", r->rax, r->rbx, r->rcx);
	printk("rdx=%0#18x,rdi=%0#18x,rsi=%0#18x\n", r->rdx, r->rdi, r->rsi);
	printk("rbp=%0#18x,r8 =%0#18x,r9 =%0#18x\n", r->rbp, r->r8, r->r9);
	printk("r10=%0#18x,r11=%0#18x,r12=%0#18x\n", r->r10, r->r11, r->r12);
	printk("r13=%0#18x,r14=%0#18x,r15=%0#18x\n", r->r13, r->r14, r->r15);
	printk("ds =%0#18x,es =%0#18x\n", r->ds, r->es);

	while(1) {}
}

void do_div_zero(long rsp, long error_code)
{
	_die(rsp, error_code, "div zero");
}

void do_debug(long rsp, long error_code)
{
	_die(rsp, error_code, "debug");
}

void do_nmi(long rsp, long error_code)
{
	_die(rsp, error_code, "nmi");
}

void do_breakpoint(long rsp, long error_code)
{
	_die(rsp, error_code, "breakpoint");
}

void do_overflow(long rsp, long error_code)
{
	_die(rsp, error_code, "overflow");
}

void do_bound_range(long rsp, long error_code)
{
	_die(rsp, error_code, "bound range");
}

void do_invalid_opcode(long rsp, long error_code)
{
	_die(rsp, error_code, "invalid opcode");
}

void do_device_not_invalid(long rsp, long error_code)
{
	_die(rsp, error_code, "device not invalid");
}

void do_double_fault(long rsp, long error_code)
{
	_die(rsp, error_code, "double fault");
}

void do_invalid_tss(long rsp, long error_code)
{
	_die(rsp, error_code, "invalid tss");
}

void do_segment_not_exsit(long rsp, long error_code)
{
	_die(rsp, error_code, "segment not exsit");
}

void do_stack_segment_fault(long rsp, long error_code)
{
	_die(rsp, error_code, "stack segment fault");
}

void do_general_protection(long rsp, long error_code)
{
	_die(rsp, error_code, "general protection");
}

void do_page_fault(long rsp, long error_code)
{
	uint64_t l;
	__asm__("movq %%cr2, %%rax" :"=a"(l));
	printk("addr=%#18x",l);
	_die(rsp, error_code, "page fault");
}

void do_x87_fpu_error(long rsp, long error_code)
{
	_die(rsp, error_code, "x87_fpu_error");
}

void do_align_check(long rsp, long error_code)
{
	_die(rsp, error_code, "align check");
}

void do_machine(long rsp, long error_code)
{
	_die(rsp, error_code, "machine check");
}

void do_SMID_fault(long rsp, long error_code)
{
	_die(rsp, error_code, "SMID fault");
}

void do_reserved_trap(long rsp, long error_code)
{
	_die(rsp, error_code, "reserved 9,15,20~31,IRQ0-31");
}

void do_default_ignore()
{
	printk("default ignore");
}
