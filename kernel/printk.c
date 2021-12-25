#define va_list                 __builtin_va_list
#define va_start(ap, fmt)       __builtin_va_start(ap,fmt)
#define va_arg(ap,type)         __builtin_va_arg(ap,type)

extern void console_printc(char c);
#define putk(c) console_printc(c)

#define F_ZEROPAD	0x01
#define F_SIGN		0x02
#define F_PLUS		0x04
#define F_SPACE		0x08
#define F_LEFT		0x10
#define F_SPECIAL	0x20

#define isdigit(c)	((c)>='0' && (c) <='9')

static inline int strlen(char*s)
{
	char *sc = s;

	while(*s++);

	return (s - sc);
}

static void puti(long num, int base, int width, int flag)
{
	char *digits = "0123456789ABCDEF";
	char c, sign, tmp[36];
	int i;

	unsigned long u_num;

	if (flag & F_LEFT)
		flag &= ~F_ZEROPAD;

	c = (flag & F_ZEROPAD) ? '0' : ' ';

	if ((flag & F_SIGN) && num < 0) {
		sign = '-';
		num = -num;

	} else
		sign = (flag & F_PLUS) ? '+' : ((flag & F_SPACE) ? ' ' : 0);

	if (flag & F_SPECIAL) {
		if (base == 16)
			width -= 2;

		else if (base == 8)
			width--;
	}

	i = 0;
	u_num = num;

	do {
		tmp[i++] = digits[u_num % base];
		u_num = u_num / base;
	} while(u_num);

	if (!(flag & (F_ZEROPAD | F_LEFT)))
		while(width-- > i)
			putk(' ');

	if (sign)
		putk(sign);

	if (flag & F_SPECIAL) {
		if (base == 8)
			putk('0');

		else if (base == 16) {
			putk('0');
			putk('x');
		}
	}

	if (!(flag & F_LEFT))
		while(width-- > i)
			putk(c);

	width -= i;

	while(i-- > 0)
		putk(tmp[i]);

	while(width-- > i)
		putk(' ');
}

void printk(const char *fmt, ...)
{
	int flag, width, l;
	va_list arg;
	char *s;

	va_start(arg, fmt);

	while(*fmt) {
		if( '%' != *fmt ) {
			putk(*fmt++);
			continue;
		}

		flag = 0;  /* get flag */
	    repeat1:
		fmt++;
		switch( *fmt ) {
		    case '-':
			flag |= F_LEFT;
			goto repeat1;

		    case '+':
			flag |= F_PLUS;
			goto repeat1;

		    case ' ':
			flag |= F_SPACE;
			goto repeat1;

		    case '#':
			flag |= F_SPECIAL;
			goto repeat1;

		    case '0':
			flag |= F_ZEROPAD;
			goto repeat1;
		}

		width = -1; /* get width */

		if (isdigit(*fmt)) {
			width = 0;

			do {
				width = width * 10 + *fmt - '0';
				fmt++;
			} while(isdigit(*fmt));

		} else if (*fmt == '*') { /* it's the next argument */
			fmt++;
			width = va_arg(arg, int);

			if ( width < 0 ) {
				width = -width;
				flag |= F_LEFT;
			}
		}

		switch(*fmt++) { /* get conv */
		    case '%':
			if (!(flag & F_LEFT))
				while (--width > 0) putk(' ');

			putk('%');

			while (--width > 0) putk(' ');

			break;

		    case 'c':
			if (!(flag & F_LEFT))
				while (--width > 0) putk(' ');

			putk((char) va_arg(arg, int));

			while (--width > 0) putk(' ');

			break;

		    case 's':
			s = va_arg(arg, char *);
			l = strlen(s);

			if (!(flag & F_LEFT))
				while (l < width--) putk(' ');

			while (*s) putk(*s++);

			while (l < width--) putk(' ');

			break;

		    case 'd':
			flag |= F_SIGN;
			puti(va_arg(arg, long), 10, width, flag);
			break;

		    case 'x':
			puti(va_arg(arg, long), 16, width, flag);
			break;

		    default:
			putk(*(fmt - 1));
		}
	}
}
