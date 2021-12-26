#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/desc.h>

extern void console_early_init();
extern void time_init();
extern void smp_init();

char pgt0[40960] __attribute__((section (".bss.pgt"))) = { 0 };
union IDTdesc64 idt_tab[256] __attribute__((section (".bss.idt"))) = { 0 };
struct GDTtable gdt_tab __attribute__((section (".bss.gdt"))) = { 0 };

static  char* mem_type_str[] = {
    "",
    "RAM",
    "ROM or Reserved",
    "ACPI Reclaim Memory",
    "ACPI NVS Memory",
    "Undefine"
};

struct E820_struct {
    uint64_t  addr;
    uint64_t  len;
    int32_t   type;
} __attribute__((packed));

void dump_e820()
{
	struct E820_struct *E820 = (struct E820_struct*)__p2v(0x500);// e820map;
	while(E820->type) {
		printk("  %#018x-%#018x, %s\n", E820->addr,
			E820->addr + E820->len,
			mem_type_str[E820->type]);
		E820++;
	}
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

static void setup_gdt()
{
	uint64_t tmp[5]={
		0,
		0x0020980000000000,
		0x0000920000000000,
		0x0020f80000000000,
		0x0000f20000000000
	};
	memcpy(&gdt_tab, &tmp, 40);
	//gdt_tab.gdt[0].value = 0;
	//gdt_tab.gdt[1].value = 0x0020980000000000   ; // Kernel Code
	//gdt_tab.gdt[2].value = 0x0000920000000000   ; // Kernel Data
	//gdt_tab.gdt[3].value = 0x0020f80000000000   ; // User Code
	//gdt_tab.gdt[4].value = 0x0000f20000000000   ; // User Data
}

static void setup_idt()
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

void cstartup()
{
	setup_gdt();
	setup_idt();

	console_early_init();
	printk("%s\n%s\n","Hello World!","Welcome to CNIX!");

	time_init();
	//__asm__("int $1");
	dump_e820();
	smp_init();
}

void mpstartup()
{
	__asm__("int $0");
	while(1){}
}
