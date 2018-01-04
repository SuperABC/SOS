#include "turbo.h"
#include <driver/ps2.h>
#include <driver/vga.h>
#include <zjunix/fs/fat.h>
#include <zjunix/slab.h>

int turboSwitch = 0;

extern int cursor_row;
extern int cursor_col;
extern int cursor_freq;

const int turboBaseX = 10, turboBaseY = 4;
const int turboSizeX = 60, turboSizeY = 22;
int turboTmpX = 0, turboTmpY = 0;

FILE cat_file;
char *wholeTxt;
int fileSize;
char turboCont[22][60] = {0};

int insertCtrl = 0;

void turbo(char *dir, char *filename){
    turboSwitch = 1;
    turboBackground();

    cursor_row = turboBaseY;
    cursor_col = turboBaseX;
    cursor_freq = 31;
    kernel_set_cursor();

    char path[64];
    kernel_strcpy(path, dir);
    kernel_strcat(path, filename);

    if (0 == fs_open(&cat_file, path)) {
        fileSize = get_entry_filesize(cat_file.entry.data);
        wholeTxt = (char *)kmalloc(fileSize + 1);
        fs_read(&cat_file, wholeTxt, fileSize);
        wholeTxt[fileSize] = 0;
        fs_close(&cat_file);
    }

    int idx = 0;
    kernel_setcolor(YELLOW, RED);
    for(int i = 0; i < fileSize; i++){
        operateEvent(wholeTxt[idx++]);
        if(turboTmpY == turboSizeY)break;
    }
    turboTmpX = turboTmpY = 0;

    while(1){
        turboKey(kernel_getkey());
        if(turboSwitch == 0)break;
    }
    
}
void turboBackground(){
	kernel_setcolor(YELLOW, RED);
	for (int i = 4; i < 26; i++) {
		for (int j = 10; j < 70; j++) {
			kernel_putch_at(' ', j, i);
		}
	}
	kernel_setcolor(BLACK, WHITE);
	kernel_puts_at("Turbo editor                                                ", 10, 3);

}
void turboKey(int k){
    if(k==0x14){
        insertCtrl = 1-insertCtrl;
    }
    k = kernel_scantoascii(k);
    
    operateEvent(k);
}

void operateEvent(int ch){
    kernel_setcolor(YELLOW, RED);
    
    if(ch == '\n' || ch == '\r'){
        turboTmpX = 0;
        turboTmpY++;
    }
    if(ch == '\t'){
        operateEvent(' ');
        operateEvent(' ');
        operateEvent(' ');
        operateEvent(' ');
    }
    if(ch >= 0x20 && ch < 0x80){
        kernel_putch_at(ch, turboBaseX + turboTmpX, turboBaseY + turboTmpY);
        turboTmpX++;
        if(turboTmpX==turboSizeX){
            turboTmpX = 0;
            turboTmpY++;
        }
    }

    cursor_row = turboBaseY + turboTmpX;
    cursor_col = turboBaseX + turboTmpY;
    kernel_set_cursor();

}
void closeTurbo(){
    for(int i = 0; i < 29; i++){
        for(int j = 0; j < 80; j++){
            kernel_putchar_at(' ', LIGHTCYAN, BLUE, i, j);
        }
    }
    turboSwitch = 0;

    kfree(wholeTxt);
}
