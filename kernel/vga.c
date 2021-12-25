#include <cnix/kernel.h>

unsigned long cur_org = __p2v(0xb8000);
int cur_x = 0;
int cur_y = 0;

static void set_cur()
{
	long p = cur_y * 80 + cur_x;
	outb(0x3d4, 14);
	outb(0x3d5, (p >> 8) & 0xff);
	outb(0x3d4, 15);
	outb(0x3d5, p & 0xff);
}

static void srcoll_up()
{
	while (cur_y >= 25) {
		memcpy((void*) cur_org, (void*) cur_org + 160, 80 * 24 * 2);
		memset((void*) cur_org + 80 * 24 * 2, 0, 80 * 2);
		cur_y--;
	}

	set_cur();
}

void console_printc(char ch)
{
	if('\r' == ch) {
		return;

	} else if('\n' == ch) {
		++cur_y;
		cur_x = 0;
		srcoll_up();
		set_cur();
	} else {
		char* p = (char*)(cur_org + (cur_y * 80 + cur_x) * 2);
		*p++ = ch;
		*p++ = 7;

		if(++cur_x >= 80) {
			cur_y++;
			cur_x = 0;
			srcoll_up();
		}

		set_cur();
	}
}

void console_early_init()
{
	unsigned char a, b;

	outb(0x3d4, 14);
	a = inb(0x3d5);
	outb(0x3d4, 15);
	b = inb(0x3d5);
	long p = (a << 8) + b;

	cur_x = p % 80;
	cur_y = p / 80;
}
