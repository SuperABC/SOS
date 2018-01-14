#ifndef _ZJUNIX_PC_H
#define _ZJUNIX_PC_H

//task state
#define _TASK_UNINIT 0
#define _TASK_READY 1
#define _TASK_RUNNING 2
#define _TASK_BLOCKED 3
#define _TASK_WAITPID 4

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
    unsigned int parent;
    unsigned int state; 
    int queue_0;
    int queue_1;
    //struct list_head sched;//用于连接到调度链表
	//struct list_head node;//用于连接到进程链表
} task_struct;

typedef union {
    task_struct task;
    unsigned char kernel_stack[4096];
} task_union;

#define PROC_DEFAULT_TIMESLOTS 6

void init_pc();
void pc_schedule(unsigned int status, unsigned int cause, context* pt_context);
int pc_peek();//unsigned int get_emptypid();
void pc_create(int asid, void (*func)(), unsigned int init_sp, unsigned int init_gp, char* name);
void pc_kill_syscall(unsigned int status, unsigned int cause, context* pt_context);
int pc_kill(int proc);
task_struct* get_curr_pcb();
int print_proc();

//update
int fork(void (*func), char* name, int queue_0);
int do_fork(char* name);
void copy_mem(int pid, char* name);
void *memcpy(void *dest, void *src, unsigned int len);

//for moniter
task_struct* get_pcb(int pid);
int get_pc_num();
int get_curr_pid();

//schedule
int queue_peek(int queue_0);
int get_curr_queue_0();

#endif  // !_ZJUNIX_PC_H