#include "main.h"
#include "desktop.h"

struct proc process[MAX_PROC];
int procNum = 0;
int procActive = 0;

void sgSetup() {
	initWindow(80, 30, "SuperGP OS", TEXT_MAP);
	initKey();
}
void sgLoop() {
	static int first = 1;
	int key;

	if (first) {
		regProc(desktopInit, desktopKey);
		first = 0;
	}

	if (biosKey(1)) {
		key = biosKey(0);

		if (process[procActive].keyFunc(key))return;

		for (int i = 0; i < procNum&&i != procActive; i++) {
			process[i].keyFunc(key);
		}
	}
	return;
}

void regProc(void (*init)(), int (*func)(int key)) {
	init();

	procActive = procNum;

	process[procNum].id = procNum;
	process[procNum++].keyFunc = func;
}
