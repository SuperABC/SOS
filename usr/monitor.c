#include "monitor.h"
#include <driver/ps2.h>
#include <driver/vga.h>

int monitorSwitch = 0;

void monitor(){
    monitorSwitch = 1;
    monitorBackground();

    while(1){
        monitorKey(kernel_getkey());
        if(monitorSwitch==0)break;
    }
}
void monitorBackground(){
	kernel_setcolor(WHITE, DARKGRAY);
	for (int i = 4; i < 26; i++) {
		for (int j = 10; j < 70; j++) {
			kernel_putch_at(' ', j, i);
		}
	}
	kernel_setcolor(BLACK, WHITE);
	kernel_puts_at("Monitor                                                     ", 10, 3);

	kernel_setcolor(WHITE, RED);
	kernel_puts_at("      Progress      ", 20, 5);
	kernel_setcolor(BLACK, LIGHTGRAY);
	kernel_puts_at("       Memory       ", 40, 5);
}
void monitorKey(int k){
    k = kernel_scantoascii(k);

	if(k=='q')closeMonitor();
}

void closeMonitor(){
    for(int i = 0; i < 29; i++){
        for(int j = 0; j < 80; j++){
            kernel_putchar_at(' ', LIGHTCYAN, BLUE, i, j);
        }
    }
    monitorSwitch = 0;
}