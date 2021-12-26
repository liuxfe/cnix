#include <cnix/kernel.h>
#include <cnix/traps.h>

extern void console_early_init();
extern void trap_init();
extern void time_init();
extern void smp_init();

char pgt0[40960] __attribute__((section (".bss.pgt"))) = { 0 };
union IDTdesc64 idt_tab[256] __attribute__((section (".bss.idt"))) = { 0 };

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

void cstartup(void)
{
	console_early_init();
	printk("%s\n%s\n","Hello World!","Welcome to CNIX!");
	trap_init();
	time_init();
	//__asm__("int $0");
	//__asm__("int $1");
	dump_e820();
	smp_init();
}

void mpstartup()
{
	while(1){}
}
