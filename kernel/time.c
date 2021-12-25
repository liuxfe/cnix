#include <cnix/kernel.h>
#include <cnix/io.h>

#define isleap(y)       ( y%400 == 0 || ( y%4 == 0 && y%100 != 0 ) )
#define bcd2int(_v)     (_v = (_v & 0x0f) + (_v >> 4) * 10)
#define rdcoms(_r,_i) 	{ outb(0x70, 0x80| _i); _r = inb(0x71); }

static int month[] = {
	0, 0,
	31,
	(31 + 28),
	(31 + 28 + 31),
	(31 + 28 + 31 + 30),
	(31 + 28 + 31 + 30 + 31),
	(31 + 28 + 31 + 30 + 31 + 30),
	(31 + 28 + 31 + 30 + 31 + 30 + 31),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
	(31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
};

static inline long mktime(int year, int mon, int day,
						  int hour, int min, int sec)
{
	long res = 0;

	for (int i = 1970; i < year; i++) {
		res += 365;

		if (isleap(i))
			res++;
	}

	res += month[mon];

	if (isleap(year) && mon > 2)
		res++;

	res = res + day - 1;
	res = res * 24 + hour;
	res = res * 60 + min;
	res = res * 60 + sec;
	return res;
}

long kstartup_timestmp = 0;

void time_init()
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
