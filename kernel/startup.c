#include <cnix/config.h>
#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/desc.h>
#include <cnix/traps.h>
#include <cnix/sched.h>

static inline void setup_i8259()
{
#if 0
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
#endif
}

extern void setup_ioapic();

static inline void setup_pic()
{
	setup_i8259();
	setup_ioapic();
}

#define bcd2int(_v)     (_v = (_v & 0x0f) + (_v >> 4) * 10)
#define rdcoms(_r,_i) 	{ outb(0x70, 0x80| _i); _r = inb(0x71); }
long kstartup_timestmp = 0;

extern long mktime(int year, int month, int day, int hour, int min, int sec);
static inline void time_init()
{
	char ylow, yhigh, month, day, hour, minute, second, sec2;

	do {
		rdcoms(second, 0x00);
		rdcoms(ylow,   0x09);
		rdcoms(yhigh,  0x32);
		rdcoms(month,  0x08);
		rdcoms(day,    0x07);
		rdcoms(hour,   0x04);
		rdcoms(minute, 0x02);
		rdcoms(sec2,   0x00);
	} while (second != sec2);

	bcd2int(ylow);
	bcd2int(yhigh);
	bcd2int(month);
	bcd2int(day);
	bcd2int(hour);
	bcd2int(minute);
	bcd2int(second);

	kstartup_timestmp = mktime(ylow + yhigh * 100,
			           month, day, hour, minute, second);
#if 1
	printk("Start time: %d-%d-%d %d:%d:%d \n",
		   ylow + yhigh * 100, month, day, hour, minute, second);
	printk("Start timestmp:%d\n", kstartup_timestmp);
#endif
}

extern void console_early_init();
extern void setup_smp();
extern void setup_cpu();
extern void setup_mem();
extern void clock_init();
extern void setup_ide();
extern void setup_lapic(long cpu_id);
extern void sched_init(long cpu_id);
extern void useable_mem();
extern void setup_kbd();
extern void time_init();
extern void cpu_init();

void __init init()
{
	printk("%s\n%s\n","Hello World!","Welcome to CNIX!");
	time_init();
	clock_init();
	useable_mem();
	__asm__ __volatile__("1:;sti;hlt;jmp 1b;");
}

void __init cstartup(long cpu_id)
{
	if(cpu_id == 0){
		console_early_init();
		setup_smp();
		setup_cpu();
		setup_mem();
		setup_pic();
		setup_kbd();
		setup_ide();

		kthread((long)init, 0, 0);
	}
	sched_init(cpu_id);
	setup_lapic(cpu_id);

	printk("CPU%d started\n", cpu_id);

	__asm__ __volatile__("1:;sti;hlt;jmp 1b;");
}
