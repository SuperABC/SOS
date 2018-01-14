#ifndef _ZJUNIX_SLAB_H
#define _ZJUNIX_SLAB_H
#include <zjunix/list.h>
#include <zjunix/buddy.h>
#define SIZE_INT 4
#define SLAB_AVAILABLE 0x0
#define SLAB_USED 0xff
struct proc_mem
{
	unsigned int mem[16];
	unsigned int pid[16];
	unsigned proc_num;
};
extern struct proc_mem pro_mem;
struct slab_head
{
    void *end_ptr;
    unsigned int nr_objs;
};
struct kmem_cache_node
{
	struct list_head partial;
	struct list_head full;
};
struct kmem_cache_cpu
{
	void **freeobj;
	struct page *page;
};
struct kmem_cache
{
	unsigned int size;
	unsigned int objsize;
	unsigned int offset;
	struct kmem_cache_node node;
	struct kmem_cache_cpu cpu;
	unsigned char name[16];
};
extern void init_slab();
extern void *kmalloc(unsigned int size);
extern void kfree(void *obj);
int get_proc_mem(int pid);
#endif
