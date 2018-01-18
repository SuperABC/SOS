#include "desktop.h"
#include "terminal.h"
#include "explorer.h"
#include "monitor.h"
#include <driver/ps2.h>
#include <driver/sd.h>
#include <driver/vga.h>
#include <zjunix/bootmm.h>
#include <zjunix/buddy.h>
#include <zjunix/fs/fat.h>
#include <zjunix/slab.h>
#include <zjunix/time.h>
#include <zjunix/utils.h>

int started = 0;
int selected = 0;

extern int monitorSwitch;

void desktop(){
    kernel_printf("Press any key to enter desktop.\n");
    kernel_getchar();//Enter desktop.
    desktopBackground();//Draw the graphic background.
    while(1){
        desktopKey(kernel_getkey());//Respons for input.
    }
}
void desktopBackground(){
    for(int i = 0; i < 29; i++){
        for(int j = 0; j < 80; j++){
            kernel_putchar_at(' ', LIGHTCYAN, BLUE, i, j);
        }
    }
    for(int i = 0; i < 80; i++){
        kernel_putchar_at(' ', BLACK, LIGHTCYAN, 29, i);
    }
    kernel_putchar_at('S', LIGHTRED, BLACK, 29, 0);
    kernel_putchar_at('t', WHITE, BLACK, 29, 1);
    kernel_putchar_at('a', WHITE, BLACK, 29, 2);
    kernel_putchar_at('r', WHITE, BLACK, 29, 3);
    kernel_putchar_at('t', WHITE, BLACK, 29, 4);
}
void desktopKey(int k){
	if (started == 0) {
		if (k == 0x14) {//Ctrl
			startList();//Open start list.
		}
	}
	else {
		if (k == 0x14) {//Ctrl
			clearStart();//Close start list.
        }
        k = kernel_scantoascii(k);//Change k to ascii.

		if (k == 'j' || k == 'J') {
			changeSelect(0);//Move down
		}
		if (k == 'k' || k == 'K') {
			changeSelect(1);//Move up.
        }
        
		if (k == '\r' || k == '\n') {
			if (selected == 0);
			else {
				clearStart();//If enter, clear start list first.
				if (selected == 1) {
                    //Not implement yet.
				}
				if (selected == 3) {
                    //Not implement yet.
				}
				if (selected == 4) {
                    terminal();//Open comman line.
                }
				if (selected == 5) {
                    explorer();//Open explorer.
                }
				if (selected == 6) {
                    monitor();//Open monitor.
                }
			}
        }
        
		if (k == 'o' || k == 'O');//O is shortcut for turn off.
		if (k == 'l' || k == 'L') {//L is shortcut for login.
			clearStart();//Not implement yet.
		}
		if (k == 's' || k == 'S') {//S is shortcut for setting.
			clearStart();//Not implement yet.
		}
		if (k == 't' || k == 'T') {//T is shortcut for terminal.
			clearStart();
            terminal();//Open comman line.
		}
		if (k == 'e' || k == 'E') {//E is shrotcut for explorer.
			clearStart();
            explorer();//Open explorer.
		}
		if (k == 'm' || k == 'M') {//M is shortcut for monitor.
			clearStart();
            monitor();//Open monitor.
		}
	}
}

void startList(){
	kernel_setcolor(BLACK, DARKGRAY);
	kernel_puts_at("Turn Off  ", 0, 28);
	kernel_puts_at("Log in    ", 0, 27);
	kernel_puts_at("          ", 0, 26);

	kernel_puts_at("Setting   ", 0, 25);
	kernel_puts_at("Terminal  ", 0, 24);
	kernel_puts_at("Explorer  ", 0, 23);
	kernel_puts_at("Monitor   ", 0, 22);
	kernel_puts_at("          ", 0, 21);

	kernel_setcolor(RED, YELLOW);
    kernel_puts_at("Start list", 0, 20);
    
	kernel_setcolor(WHITE, DARKGRAY);
	kernel_puts_at("O", 5, 28);
	kernel_puts_at("L", 0, 27);
	kernel_puts_at("S", 0, 25);
	kernel_puts_at("T", 0, 24);
	kernel_puts_at("E", 0, 23);
	kernel_puts_at("M", 0, 22);

	kernel_setcolor(BLACK, RED);
    switch(selected){
        case 0:
	        kernel_puts_at("Turn Off  ", 0, 28);
            break;
        case 1:
	        kernel_puts_at("Log in    ", 0, 27);
            break;
        case 3:
            kernel_puts_at("Setting   ", 0, 25);
            break;
        case 4:
            kernel_puts_at("Terminal  ", 0, 24);
            break;
        case 5:
            kernel_puts_at("Explorer  ", 0, 23);
            break;
        case 6:
            kernel_puts_at("Monitor   ", 0, 22);
            break;
    }

	kernel_setcolor(WHITE, RED);
    switch(selected){
        case 0:
            kernel_puts_at("O", 5, 28);
            break;
        case 1:
            kernel_puts_at("L", 0, 27);
            break;
        case 3:
            kernel_puts_at("S", 0, 25);
            break;
        case 4:
            kernel_puts_at("T", 0, 24);
            break;
        case 5:
            kernel_puts_at("E", 0, 23);
            break;
        case 6:
            kernel_puts_at("M", 0, 22);
            break;
        
    }

	started = 1;
}
void clearStart(){
    for(int i = 0; i < 29; i++){
        for(int j = 0; j < 80; j++){
            kernel_putchar_at(' ', LIGHTCYAN, BLUE, i, j);
        }
    }
    started = 0;
}
void changeSelect(int dir){
	kernel_setcolor(BLACK, DARKGRAY);
    switch(selected){
        case 0:
	        kernel_puts_at("Turn Off  ", 0, 28);
            break;
        case 1:
	        kernel_puts_at("Log in    ", 0, 27);
            break;
        case 3:
            kernel_puts_at("Setting   ", 0, 25);
            break;
        case 4:
            kernel_puts_at("Terminal  ", 0, 24);
            break;
        case 5:
            kernel_puts_at("Explorer  ", 0, 23);
            break;
        case 6:
            kernel_puts_at("Monitor   ", 0, 22);
            break;
    }

	kernel_setcolor(WHITE, DARKGRAY);
    switch(selected){
        case 0:
            kernel_puts_at("O", 5, 28);
            break;
        case 1:
            kernel_puts_at("L", 0, 27);
            break;
        case 3:
            kernel_puts_at("S", 0, 25);
            break;
        case 4:
            kernel_puts_at("T", 0, 24);
            break;
        case 5:
            kernel_puts_at("E", 0, 23);
            break;
        case 6:
            kernel_puts_at("M", 0, 22);
            break;
        
    }

	if (dir == 0) {
		if (selected > 0)selected--;
		if (selected == 2)selected--;
	}
	if (dir == 1) {
		if (selected < 6)selected++;
		if (selected == 2)selected++;
    }
    
	kernel_setcolor(BLACK, RED);
    switch(selected){
        case 0:
	        kernel_puts_at("Turn Off  ", 0, 28);
            break;
        case 1:
	        kernel_puts_at("Log in    ", 0, 27);
            break;
        case 3:
            kernel_puts_at("Setting   ", 0, 25);
            break;
        case 4:
            kernel_puts_at("Terminal  ", 0, 24);
            break;
        case 5:
            kernel_puts_at("Explorer  ", 0, 23);
            break;
        case 6:
            kernel_puts_at("Monitor   ", 0, 22);
            break;
    }

	kernel_setcolor(WHITE, RED);
    switch(selected){
        case 0:
            kernel_puts_at("O", 5, 28);
            break;
        case 1:
            kernel_puts_at("L", 0, 27);
            break;
        case 3:
            kernel_puts_at("S", 0, 25);
            break;
        case 4:
            kernel_puts_at("T", 0, 24);
            break;
        case 5:
            kernel_puts_at("E", 0, 23);
            break;
        case 6:
            kernel_puts_at("M", 0, 22);
            break;
        
    }


}
