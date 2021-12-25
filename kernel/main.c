#include <cnix/kernel.h>

extern void console_early_init();
extern void console_printc(char ch);

void disp_str(char* s){
	while(*s){
		console_printc(*s);
		s++;
	}
}

void cstartup(void){
	console_early_init();
	disp_str("Hello World!\nWelcome to CNIX!\n");
	for(;;);
}
