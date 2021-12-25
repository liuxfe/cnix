#include <cnix/kernel.h>

extern void console_early_init();

void cstartup(void){
	console_early_init();
	printk("%s\n%s\n","Hello World!","Welcome to CNIX!");
	for(;;);
}
