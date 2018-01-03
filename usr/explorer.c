#include "explorer.h"
#include "turbo.h"
#include <driver/ps2.h>
#include <driver/vga.h>
#include <zjunix/fs/fat.h>
#include <zjunix/slab.h>

int explorerSwitch = 0;

const int expBaseX = 12, expBaseY = 7;
const int expSizeX = 56, expSizeY = 18;
const int dirX = 20, dirY = 5;

char tmpDir[64] = {0};
struct itemUi itemList[64];
unsigned int startIdx = 0;
unsigned int maxIdx = 0;

int itemSelected = 0;

void explorer(){
	explorerSwitch = 1;
	clearMem();
	explorerBackground();
	
	refreshDir();
	showDir();

    while(1){
        explorerKey(kernel_getkey());
        if(explorerSwitch == 0)break;
    }
}
void explorerBackground(){
	kernel_setcolor(WHITE, DARKGRAY);
	for (int i = 4; i < 26; i++) {
		for (int j = 10; j < 70; j++) {
			kernel_putch_at(' ', j, i);
		}
	}
	kernel_setcolor(BROWN, LIGHTGRAY);
	for (int i = expBaseY; i < expBaseY+expSizeY; i++) {
		for (int j = expBaseX; j < expBaseX+expSizeX; j++) {
			kernel_putch_at(' ', j, i);
		}
	}
	kernel_setcolor(BLACK, WHITE);
	kernel_puts_at("Explorer                                                    ", 10, 3);

	kernel_strcpy(tmpDir, "/");
	kernel_setcolor(BROWN, LIGHTGRAY);
	kernel_puts_at("                                        ", dirX, dirY);
	kernel_puts_at(tmpDir, dirX, dirY);
}
void explorerKey(int k){
	k = kernel_scantoascii(k);

	int idx = itemSelected - startIdx;
	if(k=='q')closeExplorer();
	else if(k=='w'){
		if(idx < 5)return;
		else itemSelected -= 5;
		showDir();
		return;
	}
	else if(k=='s'){
		if(idx+startIdx + 5 >= maxIdx)return;

		if(maxIdx-startIdx>15){
			if(idx<10){
				itemSelected += 5;
			}
			else{
				startIdx += 5;
				itemSelected += 5;
			}
		}
		else{
			itemSelected += 5;
		}
		showDir();
		return;
	}
	else if(k=='a'){
		if(idx == 0 && startIdx == 0)return;

		if(idx!=0){
			itemSelected--;
			showDir();
			return;
		}
		else{
			startIdx -= 5;
			itemSelected--;
			showDir();
			return;
		}
	}
	else if(k=='d'){
		if(idx+startIdx==maxIdx-1)return;

		if(idx<14){
			itemSelected++;
			showDir();
			return;
		}
		else {
			itemSelected++;
			startIdx += 5;
			showDir();
			return;
		}
	}
	else if(k=='\r'||k=='\n'){
		if(itemList[itemSelected].type == IT_FOLDER){
			if(tmpDir[kernel_strlen(tmpDir)-1]!='/')
				tmpDir[kernel_strlen(tmpDir)] = '/';
			kernel_strcpy(tmpDir + kernel_strlen(tmpDir),
				itemList[itemSelected].name);
			tmpDir[kernel_strlen(tmpDir)+1] = 0;

			clearMem();

			kernel_setcolor(BLACK, LIGHTGRAY);
			kernel_puts_at("                                        ", dirX, dirY);
			kernel_puts_at(tmpDir, dirX, dirY);

			refreshDir();
			showDir();
		}
		else if(itemList[itemSelected].type == IT_TXT){
			turbo(tmpDir, itemList[itemSelected].name);

			explorerBackground();
			
			kernel_setcolor(BLACK, LIGHTGRAY);
			kernel_puts_at("                                        ", dirX, dirY);
			kernel_puts_at(tmpDir, dirX, dirY);

			refreshDir();
			showDir();
			sidePanel();
		}
	}
	else if(k==0x08){
		if(kernel_strcmp(tmpDir, "/")==0)return;

		int end = kernel_strlen(tmpDir)-1;
		while(tmpDir[end]!='/')end--;
		tmpDir[end+1] = '\0';

		clearMem();

		kernel_setcolor(BLACK, LIGHTGRAY);
		kernel_puts_at("                                        ", dirX, dirY);
		kernel_puts_at(tmpDir, dirX, dirY);

		refreshDir();
		showDir();
		sidePanel();
	}
	else if(k=='r'){//remove.
		char path[64];
		kernel_strcpy(path, tmpDir);
		kernel_strcat(path, itemList[itemSelected].name);
		fs_rm(path);
	}
	else if(k=='m'){//move

	}
	else if(k=='c'){//copy

	}
	else if(k=='n'){//rename

	}
	else if(k=='/'){//new folder

	}
	else if(k=='.'){//new file

	}
}

void refreshDir(){
    FS_FAT_DIR dir;
    struct dir_entry_attr entry;
    char name[32];
	unsigned int r;
	unsigned int idx = 0;

	if (fs_open_dir(&dir, tmpDir))return;

readdir:
	r = fs_read_dir(&dir, (unsigned char *)&entry);
    if (1 != r) {
        if (-1 != r) {
			get_filename((unsigned char *)&entry, name);
			if (entry.attr == 0x10) {
				if(kernel_strcmp(name, ".")==0)
					goto readdir;
				if(kernel_strcmp(name, "..")==0)
					goto readdir;
				kernel_strcpy(itemList[idx].name, name);
				itemList[idx].type = IT_FOLDER;
				itemList[idx].size = 0;
				itemList[idx].col = 0;
				itemList[idx].row = 0;
			}
            else {
				kernel_strcpy(itemList[idx].name, name);
				itemList[idx].type = getExt(itemList[idx].name);
				itemList[idx].size = entry.size;
				itemList[idx].col = 0;
				itemList[idx].row = 0;
			}

			idx++;
			maxIdx = idx;
			
			goto readdir;
        }
	}
}
void showDir(){
	int tmp = startIdx;

	kernel_setcolor(BROWN, LIGHTGRAY);
	for (int i = expBaseY; i < expBaseY+expSizeY; i++) {
		for (int j = expBaseX; j < expBaseX+expSizeX; j++) {
			kernel_putch_at(' ', j, i);
		}
	}
	sidePanel();

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 5; j++){
			if(tmp == maxIdx)return;

			int x = expBaseX + 8*j;
			int y = expBaseY + 6*i;

			kernel_setcolor(WHITE, BROWN);
			switch(itemList[tmp].type){
				case IT_FOLDER:
					kernel_putch_at(' ', x+2, y+1);
					kernel_putch_at(' ', x+3, y+1);
					kernel_putch_at(' ', x+4, y+1);
					kernel_putch_at(' ', x+5, y+1);
					kernel_putch_at(' ', x+2, y+2);
					kernel_putch_at(' ', x+5, y+2);
					kernel_putch_at(' ', x+2, y+3);
					kernel_putch_at(' ', x+3, y+3);
					kernel_putch_at(' ', x+4, y+3);
					kernel_putch_at(' ', x+5, y+3);
					break;
				case IT_TXT:
					kernel_putch_at(' ', x+2, y+1);
					kernel_putch_at(' ', x+3, y+1);
					kernel_putch_at(' ', x+4, y+1);
					kernel_putch_at(' ', x+5, y+1);
					kernel_putch_at(' ', x+2, y+2);
					kernel_putch_at(' ', x+5, y+2);
					kernel_putch_at(' ', x+2, y+3);
					kernel_putch_at(' ', x+3, y+3);
					kernel_putch_at(' ', x+4, y+3);
					kernel_putch_at(' ', x+5, y+3);
					
					kernel_setcolor(BROWN, WHITE);
					kernel_putch_at('=', x+3, y+2);
					kernel_putch_at('=', x+4, y+2);
					break;
				case IT_BIN:
					kernel_putch_at(' ', x+2, y+1);
					kernel_putch_at(' ', x+3, y+1);
					kernel_putch_at(' ', x+4, y+1);
					kernel_putch_at(' ', x+5, y+1);
					kernel_putch_at(' ', x+2, y+2);
					kernel_putch_at(' ', x+5, y+2);
					kernel_putch_at(' ', x+2, y+3);
					kernel_putch_at(' ', x+3, y+3);
					kernel_putch_at(' ', x+4, y+3);
					kernel_putch_at(' ', x+5, y+3);
					
					kernel_setcolor(BROWN, WHITE);
					kernel_putch_at('0', x+3, y+2);
					kernel_putch_at('1', x+4, y+2);
					break;
				case IT_OTHER:
					kernel_putch_at(' ', x+2, y+1);
					kernel_putch_at(' ', x+3, y+1);
					kernel_putch_at(' ', x+4, y+1);
					kernel_putch_at(' ', x+5, y+1);
					kernel_putch_at(' ', x+2, y+2);
					kernel_putch_at(' ', x+5, y+2);
					kernel_putch_at(' ', x+2, y+3);
					kernel_putch_at(' ', x+3, y+3);
					kernel_putch_at(' ', x+4, y+3);
					kernel_putch_at(' ', x+5, y+3);

					kernel_setcolor(BROWN, WHITE);
					kernel_putch_at('?', x+3, y+2);
					kernel_putch_at('?', x+4, y+2);
					break;
			}
			
			if(tmp==itemSelected)
				kernel_setcolor(RED, LIGHTGRAY);
			else
				kernel_setcolor(BLACK, LIGHTGRAY);

			if(kernel_strlen(itemList[tmp].name)<7){
				kernel_puts_at(itemList[tmp].name, 
					x+4-kernel_strlen(itemList[tmp].name)/2, y+4);
			}
			else{
				for(int k = 0; k < 7; k++){
					kernel_putch_at(itemList[tmp].name[k],
						x+1+k, y+4);
				}
				kernel_putch_at('-', x+7, y+4);
			}

			tmp++;
		}
	}
}
void sidePanel(){
	kernel_setcolor(BLACK, WHITE);
	for(int i = expBaseY; i < expBaseY + expSizeY; i++){
		kernel_putch_at(' ', expBaseX + 40, i);
	}

	kernel_setcolor(BLACK, LIGHTGRAY);
	kernel_puts_at("File Name:", 54, 12);
	kernel_puts_at("File Type:", 54, 15);
	kernel_puts_at("File Size:", 54, 17);
	kernel_puts_at("Last Fetch:", 54, 19);

	if(startIdx>=maxIdx)return;

	if(kernel_strlen(itemList[itemSelected].name)>12){
		for(int i = 0; i < 12; i++){
			kernel_putch_at(itemList[itemSelected].name[i], 54+i, 13);
		}
		if(kernel_strlen(itemList[itemSelected].name)>24){
			for(int i = 12; i < 24; i++){
				kernel_putch_at(itemList[itemSelected].name[i], 54+i, 14);
			}
			kernel_puts_at(itemList[itemSelected].name + 24, 54, 15);
		}
		else{
			kernel_puts_at(itemList[itemSelected].name + 12, 54, 14);
		}
	}
	else{
		kernel_puts_at(itemList[itemSelected].name, 54, 13);
	}

	switch(itemList[itemSelected].type){
		case IT_FOLDER:
			kernel_puts_at("Folder", 54, 16);
			break;
		case IT_TXT:
			kernel_puts_at("Text File", 54, 16);
			break;
		case IT_BIN:
			kernel_puts_at("Binary File", 54, 16);
			break;
		case IT_OTHER:
			kernel_puts_at("Unknown File", 54, 16);
			break;
	}
	
	kernel_putnum_at(itemList[itemSelected].size, 54, 18);
	kernel_putch_at('B', 63, 18);

}
void closeExplorer(){
    for(int i = 0; i < 29; i++){
        for(int j = 0; j < 80; j++){
            kernel_putchar_at(' ', LIGHTCYAN, BLUE, i, j);
        }
    }
    explorerSwitch = 0;
}

void clearMem(){
	startIdx = maxIdx = itemSelected = 0;
}
int getExt(char *name){
	int idx = kernel_strlen(name)-1;
	while(idx>0&&name[idx]!='.')idx--;

	if(idx==0)return IT_OTHER;
	else if(kernel_strcmp(name+idx+1, "TXT")==0)return IT_TXT;
	else if(kernel_strcmp(name+idx+1, "BIN")==0)return IT_BIN;
	else return IT_OTHER;
}