#define isleap(y)       ( y%400 == 0 || ( y%4 == 0 && y%100 != 0 ) )

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

long mktime(int year, int mon, int day, int hour, int min, int sec)
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
