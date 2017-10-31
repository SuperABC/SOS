#include "desktop.h"
#include "terminal.h"

int started = 0;
int selected = 0;

textMap *startCover;

void desktopInit() {
	backGround();
}
int desktopKey(int key) {
	if (started == 0) {
		if (key == SG_CTRL) {
			startList();
			return 1;
		}
	}
	else {
		if (key == SG_CTRL || key == 0x1B) {
			clearStart();
			return 1;
		}
		if (key == SG_DOWN) {
			changeSelect(0);
			return 1;
		}
		if (key == SG_UP) {
			changeSelect(1);
		}
		if (key == '\r') {
			if (selected == 0)exit(0);
			else {
				clearStart();
				if (selected == 2) {
					//regProc(settingInit, settingKey);
				}
				if (selected == 3) {
					regProc(terminalInit, terminalKey);
				}
				if (selected == 4) {
					//regProc(explorerInit, explorerKey);
				}
				if (selected == 5) {
					//regProc(monitorInit, monitorKey);
				}
			}
			return 1;
		}
		if (key == 'O' || key == 'o')exit(0);
		//if (key == 'S' || key == 's')regProc(settingInit, settingKey);
		if (key == 'T' || key == 't') {
			clearStart();
			regProc(terminalInit, terminalKey);
			return 1;
		}
		//if (key == 'E' || key == 'e')regProc(explorerInit, explorerKey);
		//if (key == 'M' || key == 'm')regProc(monitorInit, monitorKey);
	}
	return 0;
}

void backGround() {
	setBfc(BLUE, LIGHTCYAN);
	clearText();

	setBfc(BLACK, WHITE);
	writeString("Start", 0, 29);
	setCharColor(BLACK << 4 | LIGHTRED, 0, 29);
	setBfc(LIGHTCYAN, BLACK);
	writeString("                                                                           ", 5, 29);
}
void startList() {
	startCover = (textMap *)malloc(sizeof(textMap));
	getText(0, 21, 9, 28, startCover);

	setBfc(DARKGRAY, BLACK);
	writeString("Turn Off  ", 0, 28);
	setCharFgc(WHITE, 5, 28);
	writeString("          ", 0, 27);

	writeString("Setting   ", 0, 26);
	setCharFgc(WHITE, 0, 26);
	writeString("Terminal  ", 0, 25);
	setCharFgc(WHITE, 0, 25);
	writeString("Explorer  ", 0, 24);
	setCharFgc(WHITE, 0, 24);
	writeString("Monitor   ", 0, 23);
	setCharFgc(WHITE, 0, 23);
	writeString("          ", 0, 22);

	setBfc(YELLOW, RED);
	writeString("Start list", 0, 21);

	for (int i = 0; i < 10; i++) {
		setCharBgc(RED, i, 28 - selected);
	}

	started = 1;
}
void clearStart() {
	putText(0, 21, startCover);
	started = 0;
}
void changeSelect(int dir) {
	for (int i = 0; i < 10; i++) {
		setCharBgc(DARKGRAY, i, 28 - selected);
	}
	if (dir == 0) {
		if (selected > 0)selected--;
		if (selected == 1)selected--;
	}
	if (dir == 1) {
		if (selected < 5)selected++;
		if (selected == 1)selected++;
	}
	for (int i = 0; i < 10; i++) {
		setCharBgc(RED, i, 28 - selected);
	}
}