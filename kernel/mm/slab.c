#include <arch.h>
#include <driver/vga.h>
#include <zjunix/pc.h>
#include <zjunix/slab.h>
#include <zjunix/utils.h>

#define KMEM_ADDR(PAGE, BASE) ((((PAGE) - (BASE)) << PAGE_SHIFT) | 0x80000000)
struct proc_mem pro_mem;
struct kmem_cache kmalloc_caches[PAGE_SHIFT];

static unsigned int size_kmem_cache[PAGE_SHIFT] = {96, 192, 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048};

int get_proc_mem(int pid)
{
	int i;
	for(i = 0;i < pro_mem.proc_num;i++)
	{
		if(pro_mem.pid[i] == pid)
			return pro_mem.mem[i];
	}
	return 0;
}

void init_kmem_cpu(struct kmem_cache_cpu *kcpu)
{
	kcpu -> page = 0;
	kcpu -> freeobj = 0;
}

void init_kmem_node(struct kmem_cache_node *knode)
{
	INIT_LIST_HEAD(&(knode -> full));
	INIT_LIST_HEAD(&(knode -> partial));
}

void init_each_slab(struct kmem_cache *cache,unsigned int size)
{
	cache -> objsize = size;
	cache -> objsize += (SIZE_INT - 1);
	cache -> objsize &= ~(SIZE_INT - 1);
	cache -> size = cache -> objsize + sizeof(void *);
	cache -> offset = cache -> size;
	init_kmem_cpu(&(cache -> cpu));
	init_kmem_node(&(cache -> node));
}

void init_slab()
{
	unsigned int i;
	pro_mem.proc_num = 0;
	for(i = 0;i < 16;i++)
		pro_mem.mem[i] = pro_mem.pid[i] = 0;
	for (i = 0;i < PAGE_SHIFT;i++)
		init_each_slab(&(kmalloc_caches[i]),size_kmem_cache[i]);
	#ifdef SLAB_DEBUG
	kernel_printf("Setup Slub ok :\n");
	kernel_printf("\tcurrent slab cache size list:\n\t");
	for (i = 0; i < PAGE_SHIFT; i++)
		kernel_printf("%x %x ", kmalloc_caches[i].objsize, (unsigned int)(&(kmalloc_caches[i])));
	kernel_printf("\n");
	#endif
}
void format_slabpage(struct kmem_cache *cache, struct page *page) 
{
	unsigned char *moffset = (unsigned char *)KMEM_ADDR(page, pages);
	struct slab_head *s_head = (struct slab_head *)moffset;
	unsigned int *ptr;
	unsigned int remaining = (1 << PAGE_SHIFT);
	set_flag(page,_PAGE_SLAB);
	do
	{
		ptr = (unsigned int *)(moffset + cache -> offset);
		moffset += cache -> size;
		*ptr = (unsigned int)moffset;
		remaining -= cache -> size;
	}while (remaining >= cache -> size);
	*ptr = (unsigned int)moffset & ~((1 << PAGE_SHIFT) - 1);
	s_head -> end_ptr = ptr;
	s_head -> nr_objs = 0;
	cache -> cpu.page = page;
	cache -> cpu.freeobj = (void **)(*ptr + cache->offset);
	page -> virtual = (void *)cache;
	page -> slabp = (unsigned int)(*(cache -> cpu.freeobj));
}

void *slab_alloc(struct kmem_cache *cache)
{
	struct slab_head *s_head;
	void *object = 0;
	struct page *newpage;
	if(cache -> cpu.freeobj)
		object = *(cache -> cpu.freeobj);
	check:
	if(is_bound((unsigned int)object,(1 << PAGE_SHIFT)))
	{
		if(cache -> cpu.page)
			list_add_tail(&(cache -> cpu.page -> list),&(cache -> node.full));
		if (list_empty(&(cache->node.partial)))
		{
			newpage = __alloc_pages(0);
			if (newpage == 0)
			{
				kernel_printf("ERROR: slab request one page in cache failed\n");
				while (1);
			}
			#ifdef SLAB_DEBUG
				kernel_printf("\tnew page, index: %x \n", newpage - pages);
			#endif
			format_slabpage(cache,newpage);
			object = *(cache -> cpu.freeobj);
			goto check;
		}
		cache -> cpu.page = container_of(cache -> node.partial.next,struct page,list);
		list_del(cache -> node.partial.next);
		object = (void *)(cache -> cpu.page -> slabp);
		cache -> cpu.freeobj = (void **)((unsigned char *)object + cache -> offset);
		goto check;
	}
	cache -> cpu.freeobj = (void **)((unsigned char *)object + cache -> offset);
	cache -> cpu.page -> slabp = (unsigned int)(*(cache -> cpu.freeobj));
	s_head = (struct slab_head *)KMEM_ADDR(cache -> cpu.page, pages);
	s_head -> nr_objs++;
	if(is_bound(cache -> cpu.page -> slabp,1 << PAGE_SHIFT))
	{
		list_add_tail(&(cache -> cpu.page->list),&(cache -> node.full));
		init_kmem_cpu(&(cache -> cpu));
	}
	return object;
}

void slab_free(struct kmem_cache *cache,void *object)
{
	struct page *opage = pages + ((unsigned int)object >> PAGE_SHIFT);
	unsigned int *ptr;
	struct slab_head *s_head = (struct slab_head *)KMEM_ADDR(opage,pages);
	if (!(s_head -> nr_objs))
	{
		kernel_printf("ERROR : slab_free error!\n");
		while (1);
	}
	ptr = (unsigned int *)((unsigned char *)object + cache -> offset);
	*ptr = *((unsigned int *)(s_head -> end_ptr));
	*((unsigned int *)(s_head -> end_ptr)) = (unsigned int)object;
	s_head -> nr_objs--;
	if (list_empty(&(opage -> list)))
		return;
	else if (!(s_head -> nr_objs))
	{
		__free_pages(opage,0);
		return;
	}
	else
	{
		list_del_init(&(opage -> list));
		list_add_tail(&(opage -> list),&(cache -> node.partial));
		return;
	}
}
unsigned int get_slab(unsigned int size)
{
	unsigned int itop = PAGE_SHIFT;
	unsigned int i;
	unsigned int bf_num = (1 << (PAGE_SHIFT - 1));
	unsigned int bf_index = PAGE_SHIFT;
	for (i = 0;i < itop;i++)
	{
		if((kmalloc_caches[i].objsize >= size) && (kmalloc_caches[i].objsize < bf_num))
		{
			bf_num = kmalloc_caches[i].objsize;
			bf_index = i;
		}
	}
	return bf_index;
}

void *kmalloc(unsigned int size)
{
	int i;
	struct kmem_cache *cache;
	unsigned int bf_index;
	if (size == 0)
		return 0;
	for(i = 0;i < pro_mem.proc_num;i++)
		if(pro_mem.pid[i] == get_curr_pid())
			break;
	if(i == pro_mem.proc_num)
	{
		pro_mem.pid[pro_mem.proc_num] = get_curr_pid();
		pro_mem.mem[pro_mem.proc_num] = 0;
		pro_mem.proc_num++;
	}
	if (size > kmalloc_caches[PAGE_SHIFT - 1].objsize)
	{
		size += (1 << PAGE_SHIFT) - 1;
		size &= ~((1 << PAGE_SHIFT) - 1);
		pro_mem.mem[i] += size;
		return (void *)(KERNEL_ENTRY | (unsigned int)alloc_pages(size >> PAGE_SHIFT));
	}
	else
	{
		bf_index = get_slab(size);
		pro_mem.mem[i] += kmalloc_caches[bf_index].objsize;
		if (bf_index >= PAGE_SHIFT)
		{
			kernel_printf("ERROR: No available slab\n");
			while (1);
		}
		return (void *)(KERNEL_ENTRY | (unsigned int)slab_alloc(&(kmalloc_caches[bf_index])));
	}
}

void kfree(void *obj)
{
	int i,size;
	for(i = 0;i < pro_mem.proc_num;i++)
		if(pro_mem.pid[i] == get_curr_pid())
			break;
	struct page *page;
	obj = (void *)((unsigned int)obj & (~KERNEL_ENTRY));
	page = pages + ((unsigned int)obj >> PAGE_SHIFT);
	
	if (page -> flag != _PAGE_SLAB)
	{
		pro_mem.mem[i] -= 1 << page -> bplevel << PAGE_SHIFT;
		return free_pages((void *)((unsigned int)obj & ~((1 << PAGE_SHIFT) - 1)),page -> bplevel);
	}
	else
	{
		pro_mem.mem[i] -= ((struct kmem_cache *)(page -> virtual)) -> objsize;
		return slab_free(page -> virtual, obj);
	}
}
