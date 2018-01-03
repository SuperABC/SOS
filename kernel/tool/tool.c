#include "tool.h"

void *memcpy(void *dest, void *src, unsigned int len)
{
	unsigned char *deststr = dest;
	unsigned char *srcstr = src;
	while (len--) {
		*deststr = *srcstr;
		++deststr;
		++srcstr;
	}
	return dest;
}

void *memset(void *dest, unsigned int ch, unsigned int n)
{
	unsigned char *deststr = dest;
	
	while (n--) {
		*deststr = ch;
		++deststr;
	}
	return dest;
}

unsigned int strlen(unsigned char *str)
{
	unsigned int len = 0;
	while (str[len]) 
		++len;
	return len;
}

unsigned int mkint(short high, short low)
{
	return (high << 16) | low;
}

short mkshort(unsigned char high, unsigned char low)
{
	return (high << 8) | low;
}

unsigned int multiply(unsigned int a,unsigned int b)
{
	unsigned int res = 0;
	
	while (a != 0) {
		res += b;
		--a;
	}
	return res;
}

unsigned int division(unsigned int n, unsigned int div, unsigned int *remain)
{
	unsigned int res = 0;
	
	while ( n > div) {
		n -= div;
		++res;
	}

	*remain = n;
	return res;
}

/*
* 传入的n中必须有置位的位
*/
unsigned int highest_set(unsigned int n)
{
	unsigned int res = 0;

	while (n >> 1) {
		++res;
		n = (n >> 1);
	}

	return res;
}

unsigned int lowest_set(unsigned int n)
{
	unsigned int res = 0;

	while (!(n&1)) {
		++res;
		n = (n >> 1);
	}

	return res;
}

unsigned int is_bound(unsigned int val, unsigned int bound)
{
	return !(val & (bound - 1));
}

