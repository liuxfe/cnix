#include <cnix/kernel.h>
#include <cnix/traps.h>
#include <cnix/io.h>

static void _die(long rsp, long err_code, char* info)
{
	struct trapregs* regs = (struct trapregs*)rsp;

	printk("------- %s ---------\n", info);
	printk("ss=%0#18x,rsp=%0#18x\n", regs->ss, regs->rsp);
	printk("rflags=%0#18x\n", regs->rflags);
	printk("cs =%0#18x,rip=%0#18x\n", regs->cs, regs->rip);
	printk("rax=%0#18x,rbx=%0#18x\n", regs->rax, regs->rbx);
	printk("rcx=%0#18x,rdx=%0#18x\n", regs->rcx, regs->rdx);
	printk("rdi=%0#18x,rsi=%0#18x\n", regs->rdi, regs->rsi);
	printk("rbp=%0#18x,r8 =%0#18x\n", regs->rbp, regs->r8);
	printk("r9 =%0#18x,r10=%0#18x\n", regs->r9, regs->r10);
	printk("r11=%0#18x,r12=%0#18x\n", regs->r11, regs->r12);
	printk("r13=%0#18x,r14=%0#18x\n", regs->r13, regs->r14);
	printk("r15=%0#18x,err=%0#18x\n", regs->r15, err_code);
	printk("ds =%0#18x,es =%0#18x\n", regs->ds, regs->es);

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


void do_ignore_intr()
{
	printk("ignore intr");
}

extern void trap_div_zero();
extern void trap_debug();
extern void trap_nmi();
extern void trap_breakpoint();
extern void trap_overflow();
extern void trap_bound_range();
extern void trap_invalid_opcode();
extern void trap_device_not_invalid();
extern void trap_double_fault();
extern void trap_reserved();
extern void trap_invalid_tss();
extern void trap_segment_not_exsit();
extern void trap_stack_segment_fault();
extern void trap_general_protection();
extern void trap_page_fault();
extern void trap_x87_fpu_error();
extern void trap_align_check();
extern void trap_machine_check();
extern void trap_SIMD_fault();
extern void trap_clock_intr();
extern void trap_ignore_intr();

static inline void i8259_init()
{
	outb(0x20, 0x11);
	outb(0xa0, 0x11);
	outb(0x21, 0x20);
	outb(0xa1, 0x28);
	outb(0x21, 0x04);
	outb(0xa1, 0x02);
	outb(0x21, 0x01);
	outb(0xa1, 0x01);
	outb(0x21, 0xfb);
	outb(0xa1, 0xff);
}


void trap_init()
{
	set_trap_gate(0, (long)trap_div_zero);
	set_trap_gate(1, (long)trap_debug);
	set_trap_gate(2, (long)trap_nmi);
	set_call_gate(3, (long)trap_breakpoint);
	set_call_gate(4, (long)trap_overflow);
	set_call_gate(5, (long)trap_bound_range);
	set_trap_gate(6, (long)trap_invalid_opcode);
	set_trap_gate(7, (long)trap_device_not_invalid);
	set_trap_gate(8, (long)trap_double_fault);
	set_trap_gate(9, (long)trap_reserved);
	set_trap_gate(10, (long)trap_invalid_tss);
	set_trap_gate(11, (long)trap_segment_not_exsit);
	set_trap_gate(12, (long)trap_stack_segment_fault);
	set_trap_gate(13, (long)trap_general_protection);
	set_trap_gate(14, (long)trap_page_fault);
	set_trap_gate(15, (long)trap_reserved);
	set_trap_gate(16, (long)trap_x87_fpu_error);
	set_trap_gate(17, (long)trap_align_check);
	set_trap_gate(18, (long)trap_machine_check);
	set_trap_gate(19, (long)trap_SIMD_fault);
	for(int i=20; i<32; i++)
			set_trap_gate(i, (long)trap_reserved);
	for(int i=32; i<256; i++)
			set_intr_gate(i, (long)trap_ignore_intr);
}
