#ifndef _ZJUNIX_PC_H
#define _ZJUNIX_PC_H

#include <zjunix/list.h>

/*
* 用户地址空间的划分
* 0x4000,0000~0x8000,0000-1  user stack
* 0x8000,0000~0xC000,0000-1  user code/static data
* 0xC000,0000~0xFF00,0000-1  user heap
*
* 总结一下整体的地址空间划分
* 0 ~ _KERNEL_VIRT_END(psge.h) kernel部分
* _KERNEL_VIRT_END ~ ROM_START(arch.h) user部分
*	_KERNEL_VIRT_END ~ _TASK_CODE_START user stack
*	_TASK_CODE_START ~ _TASK_HEAP_END user code/static data
*	_TASK_HEAP_END ~ ROM_START user heap
* ROM_START ~ 0xFFFF,FFFF rom/io部分
*/

#define _TASK_CODE_START 0x80000000
#define _TASK_USER_STACK _TASK_CODE_START
#define _TASK_HEAP_END 0xC0000000

#define PAGE_SIZE 0x1000
#define KERN_STACK_BOTTOM 0x3000
#define KERN_STACK_SIZE (2 * PAGE_SIZE)

//heap stize
#define _HEAP_PAGES 16
#define _HEAP_ORDER 4
#define _HEAP_SIZE (16 * PAGE_SIZE)

//task state
#define _TASK_UNINIT 0
#define _TASK_READY 1
#define _TASK_RUNNING 2
#define _TASK_BLOCKED 3
#define _TASK_WAITPID 4

#define _DEFAULT_TICKS 2

//my_update
#define MAX_PRO_NUM 128

typedef struct {
    unsigned int epc;
    unsigned int at;
    unsigned int v0, v1;
    unsigned int a0, a1, a2, a3;
    unsigned int t0, t1, t2, t3, t4, t5, t6, t7;
    unsigned int s0, s1, s2, s3, s4, s5, s6, s7;
    unsigned int t8, t9;
    unsigned int hi, lo;
    unsigned int gp;
    unsigned int sp;
    unsigned int fp;
    unsigned int ra;
} context;

typedef struct {
    context context;
    int ASID;
    unsigned int counter;
    char name[32];
    unsigned long start_time;
    //updated
    unsigned int user_stack;//用户态堆栈栈顶指针
    unsigned int user_code;//用户态代码起始地址
    unsigned int parent;
    unsigned int state;
    unsigned int *pgd;//指向进程页表的页目录（第一级）
    struct list_head sched;//用于连接到调度链表
	struct list_head node;//用于连接到进程链表

} task_struct;

typedef union {
    task_struct task;
    unsigned char kernel_stack[4096];
} task_union;

#define PROC_DEFAULT_TIMESLOTS 6

void init_pc(void *phy_code, unsigned int phy_code_len);
void pc_schedule(unsigned int status, unsigned int cause, context* pt_context);
int pc_peek();
void pc_create(int asid, void (*func)(), unsigned int init_sp, unsigned int init_gp, char* name);
void pc_kill_syscall(unsigned int status, unsigned int cause, context* pt_context);
int pc_kill(int proc);
task_struct* get_curr_pcb();
int print_proc();

//update
void add_tasks(struct list_head *node);

#endif  // !_ZJUNIX_PC_H