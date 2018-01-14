#ifndef _ZJUNIX_BUDDY_H
#define _ZJUNIX_BUDDY_H
#include <zjunix/list.h>
#include <zjunix/lock.h>
#define _PAGE_RESERVED (1 << 31)
#define _PAGE_ALLOCED (1 << 30)
#define _PAGE_SLAB (1 << 29)
#define PAGE_SHIFT 12
#define MAX_BUDDY_ORDER 4

struct page
{
	unsigned int flag;
	unsigned int reference;
	struct list_head list;
	void *virtual;
	unsigned int bplevel;
	unsigned int slabp;
};
extern struct page *pages;
struct freelist
{
	unsigned int nr_free;
	struct list_head free_head;
};
struct buddy_sys
{
	unsigned int buddy_start_pfn;
	unsigned int buddy_end_pfn;
	struct page *start_page;
	struct lock_t lock;
	struct freelist freelist[MAX_BUDDY_ORDER + 1];
};
extern struct buddy_sys buddy;

extern void __free_pages(struct page *page, unsigned int order);
extern struct page *__alloc_pages(unsigned int order);
extern void free_pages(void *addr, unsigned int order);
extern void *alloc_pages(unsigned int order);
extern void init_buddy();
extern void buddy_info();
extern int _is_same_bplevel(struct page *p,int level);
extern void set_bplevel(struct page *p,int level);
extern void set_flag(struct page *p,int flag);
extern void clean_flag(struct page *p,int flag);
extern void set_ref(struct page *p,int flag);
extern int _is_same_bpgroup(struct page *p1,struct page *p2);
extern int has_flag(struct page *p,int flag);
extern void inc_ref(struct page *p,int flag);
extern void dec_ref(struct page *p,int flag);
#endif
