#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/desc.h>
#include <cnix/traps.h>

extern void int_div_zero();
extern void int_debug();
extern void int_nmi();
extern void int_breakpoint();
extern void int_overflow();
extern void int_bound_range();
extern void int_invalid_opcode();
extern void int_device_not_invalid();
extern void int_double_fault();
extern void int_reserved_trap();
extern void int_invalid_tss();
extern void int_segment_not_exsit();
extern void int_stack_segment_fault();
extern void int_general_protection();
extern void int_page_fault();
extern void int_x87_fpu_error();
extern void int_align_check();
extern void int_machine_check();
extern void int_SIMD_fault();
extern void int_default_ignore();

#if 0
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
#endif

static inline void setup_gdt()
{
	gdt_tab.gdt[1].value = 0x0020980000000000   ; // Kernel Code
	gdt_tab.gdt[2].value = 0x0000920000000000   ; // Kernel Data
	gdt_tab.gdt[3].value = 0x0020f80000000000   ; // User Code
	gdt_tab.gdt[4].value = 0x0000f20000000000   ; // User Data
}

static inline void setup_idt()
{
	set_trap_gate(0, (long)int_div_zero);
	set_trap_gate(1, (long)int_debug);
	set_trap_gate(2, (long)int_nmi);
	set_call_gate(3, (long)int_breakpoint);
	set_call_gate(4, (long)int_overflow);
	set_trap_gate(5, (long)int_bound_range);
	set_trap_gate(6, (long)int_invalid_opcode);
	set_trap_gate(7, (long)int_device_not_invalid);
	set_trap_gate(8, (long)int_double_fault);
	set_trap_gate(9, (long)int_reserved_trap);
	set_trap_gate(10, (long)int_invalid_tss);
	set_trap_gate(11, (long)int_segment_not_exsit);
	set_trap_gate(12, (long)int_stack_segment_fault);
	set_trap_gate(13, (long)int_general_protection);
	set_trap_gate(14, (long)int_page_fault);
	set_trap_gate(15, (long)int_reserved_trap);
	set_trap_gate(16, (long)int_x87_fpu_error);
	set_trap_gate(17, (long)int_align_check);
	set_trap_gate(18, (long)int_machine_check);
	set_trap_gate(19, (long)int_SIMD_fault);
	for(int i=20; i<32; i++)
			set_trap_gate(i, (long)int_reserved_trap);
	for(int i=32; i<256; i++)
			set_intr_gate(i, (long)int_default_ignore);
}

extern void console_early_init();
extern void setup_smp();
extern void setup_mem();
extern void setup_ioapic();
extern void clock_init();
extern void time_init();
extern void setup_ide();
extern void lapic_init(long cpu_id);
extern void sched_init(long cpu_id);
extern void useable_mem();
void __init cstartup()
{
	console_early_init();
	setup_gdt();
	setup_idt();
	setup_smp();
	setup_mem();
	setup_ioapic();

	time_init();
	clock_init();
	useable_mem();
	printk("%s\n%s\n","Hello World!","Welcome to CNIX!");
}

void __init mpstartup(long cpu_id)
{
	//lapic_init(cpu_id);
	sched_init(cpu_id);
	printk("CPU%d started\n", cpu_id);
	setup_ide();

	__asm__ __volatile__("1:;sti;hlt;jmp 1b;");
}
