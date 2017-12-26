#ifndef _LCORE_TOOL_H
#define _LCORE_TOOL_H

#define COLOR_BLACK 0 //��ɫ
#define COLOR_RED 4 //��ɫ
#define COLOR_GREEN 2 //��ɫ
#define COLOR_YELLOW 6 //��ɫ
#define COLOR_BLUE 1 //��ɫ
#define COLOR_MAGENTA 5  //���ɫ
#define COLOR_CYAN  3    //����ɫ, ��ɫ
#define COLOR_WHITE 7   //��ɫ

#define NULL 0

/*
* �ɱ�������
*/
typedef unsigned char * va_list;
#define _INTSIZEOF(n) ((sizeof(n)+sizeof(unsigned int)-1)&~(sizeof(unsigned int) - 1) )  
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) ) //��һ����ѡ������ַ  
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) ) //��һ��������ַ  
#define va_end(ap) ( ap = (va_list)0 ) // ��ָ����Ϊ��Ч  

extern void *memcpy(void *dest, void *src, unsigned int len);
extern void *memset(void *dest, unsigned int ch, unsigned int n);
extern unsigned int strlen(unsigned char *str);
extern unsigned int mkint(short high, short low);
extern short mkshort(unsigned char high, unsigned char low);
extern unsigned int multiply(unsigned int a,unsigned int b);
extern unsigned int division(unsigned int n, unsigned int div, unsigned int *remain);
extern unsigned int highest_set(unsigned int n);
extern unsigned int lowest_set(unsigned int n);
extern unsigned int is_bound(unsigned int val, unsigned int bound);
	
#endif

