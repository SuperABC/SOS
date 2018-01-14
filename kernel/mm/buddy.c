#include <driver/vga.h>
#include <zjunix/bootmm.h>
#include <zjunix/buddy.h>
#include <zjunix/list.h>
#include <zjunix/lock.h>
#include <zjunix/utils.h>

unsigned int kernel_start_pfn, kernel_end_pfn;
struct page *pages;
struct buddy_sys buddy;

int _is_same_bplevel(struct page *p,int level)
{
	if(p -> bplevel == level)
		return 1;
	else
		return 0;
}
void set_bplevel(struct page *p,int level)
{
	p -> bplevel = level;
}
void set_flag(struct page *p,int flag)
{
	p -> bplevel |= flag;
}
void clean_flag(struct page *p,int flag)
{
	p -> bplevel &= (~flag);
}
void set_ref(struct page *p,int reference)
{
	p -> reference = reference;
}
int _is_same_bpgroup(struct page *p1,struct page *p2)
{
	if(p1 -> bplevel == p2 -> bplevel)
		return 1;
	else
		return 0;
}
int has_flag(struct page *p,int flag)
{
	return p -> flag & flag;
}
void inc_ref(struct page *p,int reference)
{
	p -> reference += reference;
}
void dec_ref(struct page *p,int reference)
{
	p -> reference -= reference;
}

void buddy_info()
{
	unsigned int i;
	kernel_printf("Buddy-system :\n");
	kernel_printf("\tstart page-frame number : %x\n", buddy.buddy_start_pfn);
	kernel_printf("\tend page-frame number : %x\n", buddy.buddy_end_pfn);
	for (i = 0;i <= MAX_BUDDY_ORDER;i++)
		kernel_printf("\t(%x)# : %x frees\n",i,buddy.freelist[i].nr_free);
}
void init_pages(unsigned int start_pfn, unsigned int end_pfn)
{
	unsigned int i;
	for (i = start_pfn;i < end_pfn;i++)
	{
		clean_flag(pages + i,-1);
		set_flag(pages + i,_PAGE_RESERVED);
		(pages + i) -> reference = 1;
		(pages + i) -> virtual = (void *)(-1);
		(pages + i) -> bplevel = -1;
		(pages + i) -> slabp = 0;
		INIT_LIST_HEAD(&(pages[i].list));
	}
}

void init_buddy()
{
	unsigned int bpsize = sizeof(struct page);
	unsigned char *bp_base;
	unsigned int i;
	bp_base = bootmm_alloc_pages(bpsize * bmm.max_pfn,_MM_KERNEL,1 << PAGE_SHIFT);
	if(bp_base == 0)
	{
		kernel_printf("\nERROR : bootmm_alloc_pages failed!\nInit buddy system failed!\n");
		while(1);
	}
	pages = (struct page *)((unsigned int)bp_base | 0x80000000);
	init_pages(0,bmm.max_pfn);
	kernel_start_pfn = 0;
	kernel_end_pfn = 0;
	for (i = 0; i < bmm.cnt_infos;i++)
	{
		if (bmm.info[i].end > kernel_end_pfn)
			kernel_end_pfn = bmm.info[i].end;
	}
	kernel_end_pfn >>= PAGE_SHIFT;

	buddy.buddy_start_pfn = (kernel_end_pfn + (1 << MAX_BUDDY_ORDER) - 1) & ~((1 << MAX_BUDDY_ORDER) - 1); 
	buddy.buddy_end_pfn = bmm.max_pfn & ~((1 << MAX_BUDDY_ORDER) - 1);
	for (i = 0;i < MAX_BUDDY_ORDER + 1;i++)
	{
		buddy.freelist[i].nr_free = 0;
		INIT_LIST_HEAD(&(buddy.freelist[i].free_head));
	}
	buddy.start_page = pages + buddy.buddy_start_pfn;
	init_lock(&(buddy.lock));
	for (i = buddy.buddy_start_pfn;i < buddy.buddy_end_pfn;i++)
		__free_pages(pages + i, 0);
}

void __free_pages(struct page *pbpage, unsigned int order)
{
	unsigned int page_idx, bgroup_idx;
	unsigned int combined_idx, tmp;
	struct page *bgroup_page;

	lockup(&buddy.lock);

	page_idx = pbpage - buddy.start_page;
	// complier do the sizeof(struct) operation, and now page_idx is the index

	while(order < MAX_BUDDY_ORDER)
	{
		bgroup_idx = page_idx ^ (1 << order);
		bgroup_page = pbpage + (bgroup_idx - page_idx);
		if(_is_same_bplevel(bgroup_page, order) == 1)
		{
			list_del_init(&bgroup_page->list);
			--buddy.freelist[order].nr_free;
			set_bplevel(bgroup_page, -1);
			combined_idx = bgroup_idx & page_idx;
			pbpage += (combined_idx - page_idx);
			page_idx = combined_idx;
			order++;
		}
		else
			break;
	}
	set_bplevel(pbpage,order);
	list_add(&(pbpage->list),&(buddy.freelist[order].free_head));
	buddy.freelist[order].nr_free++;
	unlock(&buddy.lock);
}

struct page *__alloc_pages(unsigned int order)
{
	unsigned int current_order,size;
	struct freelist *free;
	struct page *page,*buddy_page;
	lockup(&buddy.lock);
	for(current_order = order;current_order <= MAX_BUDDY_ORDER;current_order++)
	{
		free = buddy.freelist + current_order;
		if (list_empty(&(free -> free_head)) == 0)
			break;
	}
	if(current_order > MAX_BUDDY_ORDER)
	{
		unlock(&buddy.lock);
		return 0;
	}
	page = container_of(free -> free_head.next,struct page,list);
	list_del_init(&(page->list));
	set_bplevel(page,order);
	set_flag(page,_PAGE_ALLOCED);
	free -> nr_free--;
	size = 1 << current_order;
	while(current_order > order)
	{
		free--;
		current_order--;
		size >>= 1;
		buddy_page = page + size;
		list_add(&(buddy_page -> list),&(free -> free_head));
		free -> nr_free ++;
		set_bplevel(buddy_page,current_order);
	}
	unlock(&buddy.lock);
	return page;
}

void *alloc_pages(unsigned int order)
{
	struct page *page = __alloc_pages(order);
	if (page == 0)
		return 0;
	else
		return (void *)((page - pages) << PAGE_SHIFT);
}

void free_pages(void *addr,unsigned int order)
{
	__free_pages(pages + ((unsigned int)addr >> PAGE_SHIFT),order);
}
