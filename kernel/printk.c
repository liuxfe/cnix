#include <cnix/config.h>
#include <cnix/kernel.h>
#include <cnix/spinlock.h>

extern void console_printc(char c);

static spinlock_t pirntk_sl = {SPIN_UNLOCK};
static char printk_buf[1024];

void printk(const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	spin_lock(&pirntk_sl);
	long i = vsprintf(printk_buf, fmt, arg);
	for(int j=0; j<i; j++)
		console_printc(printk_buf[j]);

	spin_unlock(&pirntk_sl);
}
