#ifndef _DRIVER_VGA_H
#define _DRIVER_VGA_H

#include <zjunix/utils.h>

extern int cursor_row;
extern int cursor_col;
extern int cursor_freq;

#define VGA_RED   0x00f
#define VGA_GREEN 0x0f0
#define VGA_BLUE  0xf00
#define VGA_BLACK 0x000
#define VGA_WHITE 0xfff

enum COLOR {
    BLACK = 0x000,
	BLUE = 0x700,
	GREEN = 0x070,
	CYAN = 0x770,
	RED = 0x007,
	MAGENTA = 0x707,
	BROWN = 0x077,
	DARKGRAY = 0x777,
	LIGHTGRAY = 0xbbb,
	LIGHTBLUE = 0xf00,
	LIGHTGREEN = 0x0f0,
	LIGHTCYAN = 0xff0,
	LIGHTRED = 0x00f,
	LIGHTMAGENTA = 0xf0f,
	YELLOW = 0x0ff,
	WHITE = 0xfff
};

struct screen_color {
    int front;
    int background;
};

void init_vga();
void kernel_set_cursor();
void kernel_clear_screen(int row);
void kernel_scroll_screen();
void kernel_putchar_at(int ch, int fc, int bg, int row, int col);
int kernel_putchar(int ch, int fc, int bg);
int kernel_puts(const char* s, int fc, int bg);
int kernel_putint(int i, int fc, int bg);
int kernel_vprintf(const char *format, va_list ap);
int kernel_printf(const char* format, ...);

void kernel_setcolor(int fg, int bg);
void kernel_puts_at(char *s, int x, int y);
void kernel_putch_at(char c, int x, int y);
void kernel_putnum_at(int n, int x, int y);

#endif // ! _DRIVER_VGA_H
