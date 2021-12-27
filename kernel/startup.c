#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/desc.h>
#include <cnix/traps.h>

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

extern struct{
	uint64_t entry[4096/8];
} pml4e, pdpe0, pde0, pte0;
extern long _data, _brk;

static void setup_pgt()
{
	int i;
	uint64_t p;

	pte0.entry[0] = 0x7 ;//| (1ULL<<63);
	for(i=1,p=0x1000; p<__v2p((uint64_t)&_data); p+=0x1000)
		pte0.entry[i++] = p + 0x07;
	for(p=__v2p((uint64_t)&_data); p<0x200000; p+=0x1000)
		pte0.entry[i++] = (p + 0x07) ;//| (1ULL <<63);

	pde0.entry[0] = __v2p((uint64_t)&pte0) + 0x07;
	for(i=1,p=0x200000;i<4096/8;i++,p+=0x200000)
		pde0.entry[i] = p + 0x87;

	pdpe0.entry[0] = __v2p((uint64_t)&pde0) + 0x07;
	for(i=1;i<8;i++)
		pdpe0.entry[i] = i * 1024 * 1024 * 1024ULL + 0x87;
}

long boot_cpu_id = 0;
long mem_start = (long)&_brk;

struct __attribute__((packed)){
	uint16_t limit;
	uint64_t base;
} __gdtr = {
	8192 -1, (uint64_t)&gdt_tab
}, __idtr = {
	8192 -1, (uint64_t)&idt_tab
};

static inline void _lgdt()
{
	__asm__ __volatile__("lgdt __gdtr(%%rip)"::);
	//__asm__ __volatile__("movw %%ax, %%ds"::"a"(0x10));
}

static inline void _lidt()
{
	__asm__ __volatile__("lidt __idtr(%%rip)"::);
}

extern void console_early_init();
extern void time_init();
extern void smp_init();
extern void ioapic_init();
extern void clock_init();
extern void mem_init();

extern long alloc_page();
extern long alloc_2page();

void cstartup(long cpu_id, long rsp)
{
	if(!cpu_id){
		console_early_init();
		setup_pgt();
		setup_gdt();
		setup_idt();
	}
	_lgdt();
	_lidt();

	if(!cpu_id){
		time_init();
		smp_init();
		ioapic_init();
		clock_init();
		mem_init();
		sti();
		printk("%s\n%s\n","Hello World!","Welcome to CNIX!");
		printk("cpu_id=%d, rsp=%#18x", cpu_id, rsp);

		printk("%x; %x;\n", alloc_page(), alloc_page());
		printk("%x; %x;\n", alloc_2page(), alloc_page());
		printk("%x; %x;\n", alloc_2page(), alloc_2page());
		printk("%x; %x;\n", alloc_page(), alloc_2page());
		printk("%x; %x;\n", alloc_page(), alloc_page());

		//__asm__("int $1");
	}
	while(1){}
}
