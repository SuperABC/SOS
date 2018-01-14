#include "monitor.h"
#include <zjunix/slab.h>
#include <zjunix/pc.h>
#include <driver/ps2.h>
#include <driver/vga.h>

int monitorSwitch = 0;

void monitor(){
    monitorSwitch = 1;
    monitorBackground();
    refreshProc();

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

    kernel_setcolor(BLACK, LIGHTGRAY);
    kernel_puts_at("PID PNAME       PARENT       QUEUE1  QUEUE2 ", 18, 7);
}
void monitorKey(int k){
    k = kernel_scantoascii(k);

	if(k=='q')closeMonitor();
    if(k=='p'){
        kernel_setcolor(WHITE, RED);
        kernel_puts_at("      Progress      ", 20, 5);
        kernel_setcolor(BLACK, LIGHTGRAY);
        kernel_puts_at("       Memory       ", 40, 5);

        kernel_setcolor(BLACK, LIGHTGRAY);
        kernel_puts_at("PID PNAME       PARENT       QUEUE1  QUEUE2 ", 18, 7);

        refreshProc();
    }
    if(k=='m'){
        kernel_setcolor(BLACK, LIGHTGRAY);
        kernel_puts_at("      Progress      ", 20, 5);
        kernel_setcolor(WHITE, RED);
        kernel_puts_at("       Memory       ", 40, 5);

        kernel_setcolor(BLACK, LIGHTGRAY);
        kernel_puts_at("PID PNAME       MEMORY                      ", 18, 7);

        refreshMemory();
    }
}

void closeMonitor(){
    for(int i = 0; i < 29; i++){
        for(int j = 0; j < 80; j++){
            kernel_putchar_at(' ', LIGHTCYAN, BLUE, i, j);
        }
    }
    monitorSwitch = 0;
}
void refreshProc(){
    kernel_setcolor(BLACK, LIGHTGRAY);
    for(int i = 18; i < 62; i++){
        for(int j = 8; j < 24; j++){
            kernel_putch_at(' ', i, j);
        }
    }

    int num = get_pc_num();
    for(int i = 0; i < num; i++){
        task_struct *tmp = get_pcb(i);
        kernel_putnum_at(tmp->ASID, 19, 8+i);
        kernel_puts_at(tmp->name, 23, 8+i);
        kernel_puts_at(get_pcb(tmp->parent)->name, 35, 8+i);
        
        kernel_putnum_at(tmp->queue_0, 48, 8+i);
        kernel_putnum_at(tmp->queue_1, 54, 8+i);
    }
}
void refreshMemory(){
    kernel_setcolor(BLACK, LIGHTGRAY);
    for(int i = 18; i < 62; i++){
        for(int j = 8; j < 24; j++){
            kernel_putch_at(' ', i, j);
        }
    }
    
    int num = get_pc_num();
    for(int i = 0; i < num; i++){
        task_struct *tmp = get_pcb(i);
        int mem = get_proc_mem(i);
        kernel_putnum_at(tmp->ASID, 19, 8+i);
        kernel_puts_at(tmp->name, 23, 8+i);
        kernel_putnum_at(mem, 35, 8+i);
    }
}