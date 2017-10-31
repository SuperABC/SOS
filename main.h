#ifndef SOS_MAIN
#define SOS_MAIN

#include "winsgl.h"
#pragma comment(lib, SG_LIB("winsgl"))

#define MAX_PROC 256

struct proc {
	int id;
	int(*keyFunc)(int key);
};

void regProc(void(*init)(), int(*func)(int key));

#endif