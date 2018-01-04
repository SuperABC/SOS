#include "terminal.h"
#include "cmd/basic.h"
#include <driver/ps2.h>
#include <driver/vga.h>
#include <zjunix/fs/fat.h>
#include <zjunix/slab.h>

int terminalSwitch = 0;

extern int cursor_row;
extern int cursor_col;
extern int cursor_freq;

int terminalCurX = 0, terminalCurY = 0;//当前光标位置，需要与cursor同步，跟随一切输出函数改变。
int terminalBaseX = 8, terminalBaseY = 3;//Terminal左上角坐标，常数。
int terminalSizeX = 64, terminalSizeY = 24;//Terminal尺寸，常数。
int terminalCmdX = 5, terminalCmdY = 0;//Terminal指令起始位置，指令执行完毕后改变。

char terminalCont[64][24] = {0};

char inst[256];
int len = 0;

void terminal(){
    terminalSwitch = 1;
    terminalBackground();

	for (int i = 0; i < 256; i++)inst[i] = 0;
    len = 0;
    
    while(1){
        terminalKey(kernel_getkey());
        if(terminalSwitch==0)break;
    }
}
void terminalBackground(){
	kernel_setcolor(WHITE, BLACK);
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 24; j++) {
            kernel_putchar_at(' ', WHITE, BLACK, 3 + j, 8 + i);
            terminalCont[i][j] = 0;
		}
	}
	kernel_setcolor(BLACK, WHITE);
	kernel_puts_at("Terminal --written by Super GP                                  ", 8, 2);
	kernel_setcolor(WHITE, BLACK);
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
    cursor_freq = 31;
    kernel_set_cursor();
}
void terminalKey(int k){
    k = kernel_scantoascii(k);

	if (k >= 0x20 && k < 0x80) {
		kernel_setcolor(WHITE, BLACK);
		if (terminalCurX < terminalSizeX - 1) {
            kernel_putch_at(k, terminalBaseX + terminalCurX, terminalBaseY + terminalCurY);
            terminalCont[terminalCurX][terminalCurY] = k;
            
            terminalCurX++;
            cursor_col = terminalBaseX + terminalCurX;
            kernel_set_cursor();
		}
		else {
			if (terminalCurY < terminalSizeY - 1) {
				kernel_putch_at(k, terminalBaseX + terminalCurX, terminalBaseY + terminalCurY);
                terminalCont[terminalCurX][terminalCurY] = k;
                
				terminalCurX = 0;
				terminalCurY++;
                cursor_col = terminalBaseX + terminalCurX;
                cursor_row = terminalBaseY + terminalCurY;
                kernel_set_cursor();
			}
			else {
				kernel_putch_at(k, terminalBaseX + terminalCurX, terminalBaseY + terminalCurY);
                terminalCont[terminalCurX][terminalCurY] = k;
                rollScrn();
                
				terminalCurX = 0;
                cursor_col = terminalBaseX + terminalCurX;
                kernel_set_cursor();
			}
		}
		inst[len++] = k;
	}
	if (k == 0x08) {
		if (terminalCurX == terminalCmdX&&terminalCurY == terminalCmdY)return;
		if (terminalCurX > 0) {
			kernel_putch_at(' ', terminalBaseX + terminalCurX - 1, terminalBaseY + terminalCurY);
            terminalCont[terminalCurX][terminalCurY] = 0;

			terminalCurX--;
            cursor_col = terminalBaseX + terminalCurX;
            kernel_set_cursor();
		}
		else {
			kernel_putch_at(' ', terminalBaseX + terminalSizeX - 1, terminalBaseY + terminalCurY - 1);
            terminalCont[terminalCurX][terminalCurY] = 0;

			terminalCurX = terminalSizeX - 1;
			terminalCurY--;
            cursor_col = terminalBaseX + terminalCurX;
            cursor_row = terminalBaseY + terminalCurY;
            kernel_set_cursor();
		}
		inst[len--] = 0;
	}
	if (k == '\r' || k == '\n') {
		terminalCurX = 0;
		if (terminalCurY == terminalSizeY - 1)rollScrn();
        else terminalCurY++;
        cursor_col = terminalBaseX + terminalCurX;
        cursor_row = terminalBaseY + terminalCurY;
        kernel_set_cursor();

		interpreteCmd();

		for (int i = 0; i < 256; i++)inst[i] = 0;
		len = 0;
	}
}

void terminalPrint(char *s, int x, int y){
	if (kernel_strlen(s) < terminalSizeX - x) {
        kernel_puts_at(s, terminalBaseX + x, terminalBaseY + y);
        for(int i = 0; i < kernel_strlen(s); i++){
            terminalCont[x+i][y] = s[i];
        }
        terminalCurX = x + kernel_strlen(s);

        cursor_col = terminalBaseX + terminalCurX;
        cursor_row = terminalBaseY + terminalCurY;
        cursor_freq = 31;
        kernel_set_cursor();
	}
	else {
		int i;
		for (i = x; i < terminalSizeX; i++) {
            kernel_putch_at(s[i - x], terminalBaseX + i, terminalBaseY + y);
            terminalCont[i][y] = s[i-x];
		}
		if (y == terminalSizeY - 1)rollScrn();
		else {
			y++;
            terminalCurY++;
            terminalCurX = 0;
		}
		terminalPrint(s + terminalSizeX - x, 0, y);
	}
}
void rollScrn(){
	char c;
	for (int i = 1; i < 24; i++) {
		for (int j = 0; j < 64; j++) {
            c = terminalCont[j][i];
			kernel_putch_at(c, terminalBaseX + j, terminalBaseY + i - 1);
            terminalCont[j][i-1] = c;
		}
	}
	for (int i = 0; i < 64; i++) {
		kernel_putch_at(' ', terminalBaseX + i, terminalBaseY + terminalSizeY - 1);
        terminalCont[i][23] = ' ';
	}
}
void finishLine(){
	if (terminalCurY == terminalSizeY - 1)rollScrn();
    else terminalCurY++;
    
    terminalCurX = 0;
	kernel_puts_at("SOS >", terminalBaseX + terminalCurX, terminalBaseY + terminalCurY);
    terminalCont[terminalCurX++][terminalCurY] = 'S';
    terminalCont[terminalCurX++][terminalCurY] = 'O';
    terminalCont[terminalCurX++][terminalCurY] = 'S';
    terminalCont[terminalCurX++][terminalCurY] = ' ';
    terminalCont[terminalCurX++][terminalCurY] = '>';
    terminalCmdX = terminalCurX;
    terminalCmdY = terminalCurY;

    cursor_col = terminalBaseX + terminalCurX;
    cursor_row = terminalBaseY + terminalCurY;
    cursor_freq = 31;
    kernel_set_cursor();

}
void interpreteCmd(){
    if (inst[0] == 0){
        finishLine();
        return;
    }
    
    char *insturction = cmdInst(inst);

    if (kernel_strcmp(insturction, "exit") == 0)closeTerminal();
    else if (kernel_strcmp(insturction, "clear") == 0)clear();
    else if (kernel_strcmp(insturction, "version") == 0)version();
    else if (kernel_strcmp(insturction, "sleep") == 0)delay(cmdPara(inst));
    else if (kernel_strcmp(insturction, "echo") == 0)echo(cmdPara(inst));
	else nocmd(); 
}
void closeTerminal(){
    for(int i = 0; i < 29; i++){
        for(int j = 0; j < 80; j++){
            kernel_putchar_at(' ', LIGHTCYAN, BLUE, i, j);
        }
    }
    terminalSwitch = 0;

    cursor_freq = 0;
    kernel_set_cursor();
}
char *cmdInst(char *cmd){
    int i = 0;
    char *ret = kmalloc(64);
    while(cmd[i]!=' '&&cmd[i]!='\0'){
        ret[i] = cmd[i];
        i++;
    }
    ret[i] = 0;
    return ret;
}
char *cmdPara(char *cmd){
    int i = 0;
    char *ret = kmalloc(64);
    while(*(cmd++)!=' '&&cmd[i]!='\0');

    while(cmd[i]!='\0'){
        ret[i] = cmd[i];
        i++;
    }
    ret[i] = 0;
    return ret;
}
