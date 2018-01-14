#include "basic.h"
#include "../terminal.h"
#include <driver/ps2.h>
#include <driver/vga.h>
#include <zjunix/fs/fat.h>
#include <zjunix/slab.h>

extern int cursor_row;
extern int cursor_col;

extern int terminalCurX, terminalCurY;
extern int terminalBaseX, terminalBaseY;
extern int terminalSizeX, terminalSizeY;
extern int terminalCmdX, terminalCmdY;

extern char terminalCont[64][24];

extern char inst[256];
extern int len;

void clear(){
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 24; j++) {
            kernel_putchar_at(' ', WHITE, BLACK, 3 + j, 8 + i);
            terminalCont[i][j] = 0;
		}
	}
    kernel_puts_at("SOS >", terminalBaseX, terminalBaseY);
    terminalCont[0][0] = 'S';
    terminalCont[1][0] = 'O';
    terminalCont[2][0] = 'S';
    terminalCont[3][0] = ' ';
    terminalCont[4][0] = '>';

	terminalCurX = 5;
    terminalCurY = 0;
    terminalCmdX = terminalCurX;
    terminalCmdY = terminalCurY;
    cursor_col = terminalBaseX + terminalCurX;
    cursor_row = terminalBaseY + terminalCurY;
    kernel_set_cursor();
}
void version() {
	terminalPrint("SOS(Super Operating System)", terminalCurX, terminalCurY);

	terminalCurX = 0;
	if (terminalCurY == terminalSizeY - 1)rollScrn();
	else terminalCurY++;
	terminalPrint("Coded by SG, GS, SS.", terminalCurX, terminalCurY);

	terminalCurX = 0;
	if (terminalCurY == terminalSizeY - 1)rollScrn();
	else terminalCurY++;
	terminalPrint("Version v0.0. However, it's not a complete OS.", terminalCurX, terminalCurY);

	terminalCurX = 0;
	if (terminalCurY == terminalSizeY - 1)rollScrn();
	else terminalCurY++;
	terminalPrint("We will add many other features next version.", terminalCurX, terminalCurY);

	terminalCurX = 0;
	if (terminalCurY == terminalSizeY - 1)rollScrn();
	else terminalCurY++;
	if (terminalCurY == terminalSizeY - 1)rollScrn();
	else terminalCurY++;
	terminalPrint("All rights reserved. 2017.10.30", terminalCurX, terminalCurY);

	finishLine();
}
void delay(char *c){
	int idx = 0;
	int sum = 0;
	while(c[idx]){
		sum = sum*10+c[idx++];
	}

	while(sum--);
	kfree(c);
}
void echo(char *c){
	terminalPrint(c, terminalCurX, terminalCurY);
}
void nocmd() {
    terminalPrint(inst, terminalCurX, terminalCurY);
	terminalPrint(" is not a command or a executable program.", terminalCurX, terminalCurY);
	finishLine();
}
void ps(){
    /*int i;
    kernel_puts_at("PID name queue_0 queue_1\n", terminalCurX, terminalCurY);
    for (i = 0; i < get_pc_num; i++) {
        if (pcb[i].ASID >= 0){
			kernel_put_num()
		}
            kernel_printf(" %x  %s  %d  %d\n", pcb[i].ASID, pcb[i].name, pcb[i].queue_0, pcb[i].queue_1);
    }
    return 0;*/
}

