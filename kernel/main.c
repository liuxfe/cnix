#include <cnix/kernel.h>

extern void console_early_init();
extern void trap_init();

void cstartup(void){
	console_early_init();
	printk("%s\n%s\n","Hello World!","Welcome to CNIX!");
	trap_init();
	//__asm__("int $0");
	__asm__("int $1");
	for(;;);
}
