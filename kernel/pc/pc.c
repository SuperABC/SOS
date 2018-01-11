#include "pc.h"

#include <driver/vga.h>
#include <intr.h>
#include <zjunix/syscall.h>
#include <zjunix/utils.h>
#include <zjunix/slab.h>

#define MAX_PID 128

task_struct pcb[MAX_PID];
int curr_proc;

//调度队列的优先级：0 > 1 > 2
int queue[3][MAX_PID];//保存进程号
int curr_queue_0;//当前进程在哪一个队列
int curr_queue_1;//当前进程在该队列的几号位

static void copy_context(context* src, context* dest) {
    dest->epc = src->epc;
    dest->at = src->at;
    dest->v0 = src->v0;
    dest->v1 = src->v1;
    dest->a0 = src->a0;
    dest->a1 = src->a1;
    dest->a2 = src->a2;
    dest->a3 = src->a3;
    dest->t0 = src->t0;
    dest->t1 = src->t1;
    dest->t2 = src->t2;
    dest->t3 = src->t3;
    dest->t4 = src->t4;
    dest->t5 = src->t5;
    dest->t6 = src->t6;
    dest->t7 = src->t7;
    dest->s0 = src->s0;
    dest->s1 = src->s1;
    dest->s2 = src->s2;
    dest->s3 = src->s3;
    dest->s4 = src->s4;
    dest->s5 = src->s5;
    dest->s6 = src->s6;
    dest->s7 = src->s7;
    dest->t8 = src->t8;
    dest->t9 = src->t9;
    dest->hi = src->hi;
    dest->lo = src->lo;
    dest->gp = src->gp;
    dest->sp = src->sp;
    dest->fp = src->fp;
    dest->ra = src->ra;
}

void init_pc() {
    int i;
    for (i = 1; i < MAX_PID; i++)
        pcb[i].ASID = -1;
    int j;
    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < MAX_PID; i++)
        queue[j][i] = -1;
    }

    pcb[0].state = _TASK_UNINIT;
    pcb[0].ASID = 0;
    pcb[0].counter = PROC_DEFAULT_TIMESLOTS;
    kernel_strcpy(pcb[0].name, "init");
    pcb[0].parent = 0;
    pcb[0].state = _TASK_RUNNING;

    curr_proc = 0;

    queue[2][0] = 0;
    curr_queue_0 = 2;
    curr_queue_1 = 0;

    register_syscall(10, pc_kill_syscall);
    register_interrupt_handler(7, pc_schedule);

    asm volatile(
        "li $v0, 1000000\n\t"
        "mtc0 $v0, $11\n\t"
        "mtc0 $zero, $9");
}
/*
void pc_schedule(unsigned int status, unsigned int cause, context* pt_context) {
    // Save context
    copy_context(pt_context, &(pcb[curr_proc].context));
    pcb[curr_proc].state = _TASK_READY;
    int i;
    for (i = 0; i < MAX_PID; i++) {
        curr_proc = (curr_proc + 1) & (MAX_PID-1);
        if (pcb[curr_proc].ASID >= 0)
            break;
    }

    if (i == MAX_PID) {
        kernel_puts("Error: PCB[0] is invalid!\n", 0xfff, 0);
        while (1)
            ;
    }
    // Load context
    copy_context(&(pcb[curr_proc].context), pt_context);
    pcb[curr_proc].state = _TASK_RUNNING;
    asm volatile("mtc0 $zero, $9\n\t");
}
*/

void pc_schedule_2(unsigned int status, unsigned int cause, context* pt_context) {
    // Save context
    copy_context(pt_context, &(pcb[curr_proc].context));
    pcb[curr_proc].state = _TASK_READY;
    int i;
    for (i = 0; i < MAX_PID; i++) {
        curr_queue_1 = (curr_queue_1 + 1) & (MAX_PID-1);
        if (queue[curr_queue_0][curr_queue_1] >= 0)
        {
            curr_proc = queue[curr_queue_0][curr_queue_1];
            break;
        }
    }

    if (i == MAX_PID) {
        kernel_puts("Error: PCB[0] is invalid!\n", 0xfff, 0);
        while (1)
            ;
    }
    // Load context
    copy_context(&(pcb[curr_proc].context), pt_context);
    pcb[curr_proc].state = _TASK_RUNNING;
    asm volatile("mtc0 $zero, $9\n\t");
}

void pc_schedule(unsigned int status, unsigned int cause, context* pt_context)
{
    int i,j;
    pc_schedule_2(status,cause,pt_context);
}

int pc_peek() {
    int i = 0;
    for (i = 0; i < MAX_PID; i++)
        if (pcb[i].ASID < 0)
            break;
    if (i == MAX_PID)
        return -1;
    return i;
}

void pc_create(int asid, void (*func)(), unsigned int init_sp, unsigned int init_gp, char* name) {
    pcb[asid].state = _TASK_UNINIT;

    pcb[asid].parent = 0;
    pcb[asid].context.epc = (unsigned int)func;
    pcb[asid].context.sp = init_sp;
    pcb[asid].context.gp = init_gp;
    kernel_strcpy(pcb[asid].name, name);
    pcb[asid].ASID = asid;
    
    pcb[asid].state = _TASK_RUNNING;
    
    int queue_1 = queue_peek(2);
    if( queue_1 == -1)
    {
        kernel_puts("Failed to allocate queue\n", 0xfff, 0);
    }
    queue[2][queue_1] = asid;
    curr_queue_0 = 2;
    curr_queue_1 = queue_1;
    
}

void pc_kill_syscall(unsigned int status, unsigned int cause, context* pt_context) {
    if (curr_proc != 0) {
        pcb[curr_proc].ASID = -1;
        pc_schedule(status, cause, pt_context);
    }
}

int pc_kill(int proc) {
    proc &= 7;
    if (proc != 0 && pcb[proc].ASID >= 0) {
        pcb[proc].ASID = -1;
        return 0;
    } else if (proc == 0)
        return 1;
    else
        return 2;
}

task_struct* get_curr_pcb() {
    return &pcb[curr_proc];
}

int print_proc() {
    int i;
    kernel_puts("PID name\n", 0xfff, 0);
    for (i = 0; i < MAX_PID; i++) {
        if (pcb[i].ASID >= 0)
            kernel_printf(" %x  %s\n", pcb[i].ASID, pcb[i].name);
    }
    return 0;
}

//mine !!!
int fork(void (*func), char* name)
{
    int asid = pc_peek();
    if (asid < 0) {
        kernel_puts("Failed to allocate pid.\n", 0xfff, 0);
        return -1;
    }
    unsigned int init_gp;
    asm volatile("la %0, _gp\n\t" : "=r"(init_gp));
    //pc_create(asid, test_proc, (unsigned int)kmalloc(4096), init_gp, "test");
    pcb[asid].state = _TASK_UNINIT;

    pcb[asid].parent = curr_proc;
    pcb[asid].context.epc = (unsigned int)func;
    pcb[asid].context.sp = (unsigned int)kmalloc(4096);
    if (pcb[asid].context.sp == 0) {
        kernel_puts("Failed to allocate stack space.\n", 0xfff, 0);
        return -1;
    }
    pcb[asid].context.gp = init_gp;
    kernel_strcpy(pcb[asid].name, name);
    pcb[asid].ASID = asid;
    
    pcb[asid].state = _TASK_RUNNING;

    int queue_1 = queue_peek(2);
    if( queue_1 == -1)
    {
        kernel_puts("Failed to allocate queue\n", 0xfff, 0);
    }
    queue[2][queue_1] = asid;
    curr_queue_0 = 2;
    curr_queue_1 = queue_1;

    return 0;
}

int do_fork(char* name)
{
	int res;
    
    //context *ctx = pcb[curr_proc].context;

    int pid = pc_peek();
    if (pid < 0) {
        kernel_puts("Failed to allocate pid.\n", 0xfff, 0);
        return -1;
    }

	copy_mem(pid, name);
	
    res = pcb[pid].ASID;
	pcb[curr_proc].context.v0 = 0;
	
	return res;
}

void copy_mem(int pid, char* name)
{
    pcb[pid].state = _TASK_UNINIT;

    memcpy(&pcb[pid],&pcb[curr_proc],sizeof(task_struct));
    kernel_strcpy(pcb[pid].name, name);
    pcb[pid].ASID = pcb[curr_proc].ASID;
    pcb[pid].parent = curr_proc;
    pcb[pid].counter = PROC_DEFAULT_TIMESLOTS;

    unsigned int init_gp;
    unsigned int init_sp = (unsigned int)kmalloc(4096);
    if(!init_sp){
        kernel_puts("Failed to allocate memory.\n", 0xfff, 0);
    }
    asm volatile("la %0, _gp\n\t" : "=r"(init_gp));
    pcb[pid].context.sp = init_sp;
    pcb[pid].context.gp = init_gp;

    pcb[pid].state = _TASK_RUNNING;
}

void *memcpy(void *dest, void *src, unsigned int len)
{
	unsigned char *deststr = dest;
	unsigned char *srcstr = src;
	while (len--) {
		*deststr = *srcstr;
		++deststr;
		++srcstr;
	}
	return dest;
}

task_struct* get_pcb(int pid) {
    return &pcb[pid];
}

int get_pc_num() {
    int i = 0;
    int num = 0;
    for (i = 0; i < MAX_PID; i++)
        if (pcb[i].ASID >= 0)
            num++;
    return num;
}

int get_curr_pid()
{
    return curr_proc;
}

int queue_peek(int queue_0)
{
    int i = 0;
    for (i = 0; i < MAX_PID; i++)
        if (queue[queue_0][i] < 0)
            break;
    if (i == MAX_PID)
        return -1;
    return i;
}
