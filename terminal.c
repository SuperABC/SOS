#include "terminal.h"

int curX = 0, curY = 0;
int baseX = 8, baseY = 3;
int sizeX = 64, sizeY = 24;

void terminalInit() {
	blackFrame();
}
int terminalKey(int key) {
	inputKey(key);
	return 0;
}

void blackFrame() {
	setBfc(BLACK, WHITE);
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 24; j++) {
			writeChar(' ', 8 + i, 3 + j);
		}
	}
	setBfc(WHITE, BLACK);
	writeString("Terminal --written by Super GP                                  ", 8, 2);

	setBfc(BLACK, LIGHTGRAY);
	writeString("SOS >_", 8, 3);

	curX = 5;
	curY = 0;
}
void inputKey(int key) {
	if (key >= 0x20 && key < 0x80) {
		setBfc(BLACK, WHITE);
		if (curX < sizeX - 1) {
			writeChar(key, baseX + curX, baseY + curY);
			writeChar('_', baseX + curX + 1, baseY + curY);
			curX++;
		}
		else {
			writeChar(key, baseX + curX, baseY + curY);
			writeChar('_', baseX, baseY + curY + 1);
			curX = 0;
			curY++;
		}
	}
}