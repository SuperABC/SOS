#ifndef _LCORE_TOOL_H
#define _LCORE_TOOL_H

#define COLOR_BLACK 0 //黑色
#define COLOR_RED 4 //红色
#define COLOR_GREEN 2 //绿色
#define COLOR_YELLOW 6 //黄色
#define COLOR_BLUE 1 //蓝色
#define COLOR_MAGENTA 5  //洋红色
#define COLOR_CYAN  3    //蓝绿色, 青色
#define COLOR_WHITE 7   //白色

#define NULL 0

/*
* 可变参数相关
*/
typedef unsigned char * va_list;
#define _INTSIZEOF(n) ((sizeof(n)+sizeof(unsigned int)-1)&~(sizeof(unsigned int) - 1) )  
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) ) //第一个可选参数地址  
#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) ) //下一个参数地址  
#define va_end(ap) ( ap = (va_list)0 ) // 将指针置为无效  

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

