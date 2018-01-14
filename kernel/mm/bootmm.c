#include <arch.h>
#include <driver/vga.h>
#include <zjunix/bootmm.h>
#include <zjunix/utils.h>

struct bootmm bmm;

char *mem_msg[] = {"Kernel code/data", "Mm Bitmap", "Vga Buffer", "Kernel page directory", "Kernel page table", "Dynamic", "Reserved"};

void set_mminfo(struct bootmm_info *info, unsigned int start, unsigned int end, unsigned int type)
{
	info -> start = start;
	info -> end = end;
	info -> type = type;
}
unsigned char bootmmmap[MACHINE_MMSIZE >> PAGE_SHIFT];

unsigned int insert_mminfo(struct bootmm *mm, unsigned int start, unsigned int end, unsigned int type) {
	unsigned int i;
	for (i = 0;i < mm -> cnt_infos;i++)
	{
		if (mm->info[i].type == type)
		{
			if (mm -> info[i].end == start - 1)
			{
				if((i + 1) < mm -> cnt_infos)
				{
					if (mm->info[i + 1].type != type)
					{
						mm -> info[i].end = end;
						return 2;//insert forward-connecting segment
					}
					else
					{
						if(mm->info[i + 1].start - 1 == end)
						{
							mm->info[i].end = mm->info[i + 1].end;
							remove_mminfo(mm, i + 1);
							return 7;//insert bridge-connecting segment
						}
					}
				}
				else
				{
					mm->info[i].end = end;
					return 6;//insert forward-connecting segment to after-last position
				}
			}
			if (mm->info[i].start - 1 == end)
			{
				mm->info[i].start = start;
				return 4;//insert following-connecting segment
			}
		}
	}

	if(mm->cnt_infos >= MAX_INFO)
		return 0;//insert_mminfo failed
	set_mminfo(mm -> info + mm -> cnt_infos,start,end,type);
	mm -> cnt_infos++;
	return 1;// insert non-related mm_segment
}

// get one sequential memory area to be split into two parts
unsigned int split_mminfo(struct bootmm *mm,unsigned int index,unsigned int split_start)
{
	unsigned int i;
	split_start &= PAGE_ALIGN;
	if((split_start <= mm -> info[index].start) || (split_start >= mm -> info[index].end))
		return 0;
	if(mm->cnt_infos == MAX_INFO)
		return 0;
	for(i = mm -> cnt_infos - 1; i >= index; i--)
		mm -> info[i + 1] = mm -> info[i];
	mm -> info[index].end = split_start - 1;
	mm -> info[index + 1].start = split_start;
	mm -> cnt_infos++;
	return 1;
}

void remove_mminfo(struct bootmm *mm,unsigned int index)
{
	unsigned int i;
	if(index >= mm -> cnt_infos)
		return;
	if(index + 1 < mm->cnt_infos)
	{
		for(i = (index + 1);i < mm -> cnt_infos;i++)
			mm->info[i - 1] = mm->info[i];
	}
	mm -> cnt_infos--;
}

void init_bootmm()
{
	unsigned int index;
	unsigned char *t_map;
	unsigned int end;
	end = 16 * 1024 * 1024;
	kernel_memset(&bmm,0,sizeof(bmm));
	bmm.phymm = get_phymm_size();
	bmm.max_pfn = bmm.phymm >> PAGE_SHIFT;
	bmm.s_map = bootmmmap;
	bmm.e_map = bootmmmap + sizeof(bootmmmap);
	bmm.cnt_infos = 0;
	kernel_memset(bmm.s_map,PAGE_FREE,sizeof(bootmmmap));
	insert_mminfo(&bmm,0,(unsigned int)(end - 1),_MM_KERNEL);
	bmm.last_alloc_end = (((unsigned int)(end) >> PAGE_SHIFT) - 1);
	for(index = 0;index < end >> PAGE_SHIFT;index++)
		bmm.s_map[index] = PAGE_USED;
}

void set_maps(unsigned int s_pfn,unsigned int cnt,unsigned char value)
{
	unsigned int i;
	for(i = 0;i < cnt;i++)
	{
		bmm.s_map[s_pfn] = (unsigned char)value;
		s_pfn++;
	}
}

unsigned char *find_pages(unsigned int page_cnt,unsigned int s_pfn,unsigned int e_pfn,unsigned int align_pfn)
{
	unsigned int index,i;
	s_pfn += (align_pfn - 1);
	s_pfn &= ~(align_pfn - 1);

	for(index = s_pfn;index < e_pfn;)
	{
		if(bmm.s_map[index] == PAGE_USED)
		{
			index++;
			continue;
		}
		for(i = index;i < index + page_cnt;i++)
		{
			if (i >= e_pfn)
				return 0;
			if (bmm.s_map[i] == PAGE_USED)
				break;
		}
		if(i == index + page_cnt)
		{
			bmm.last_alloc_end = i - 1;
			set_maps(index,page_cnt,PAGE_USED);
			return (unsigned char *)(index << PAGE_SHIFT);
		}
		else 
			index = i + align_pfn;
	}
	return 0;
}

unsigned char *bootmm_alloc_pages(unsigned int size,unsigned int type,unsigned int align)
{
	unsigned int size_inpages;
	unsigned char *res;

	size += ((1 << PAGE_SHIFT) - 1);
	size &= PAGE_ALIGN;
	size_inpages = size >> PAGE_SHIFT;
	//find at last allocation position
	res = find_pages(size_inpages,bmm.last_alloc_end + 1,bmm.max_pfn,align >> PAGE_SHIFT);
	if(res != 0)
	{
		insert_mminfo(&bmm,(unsigned int)res,(unsigned int)res + size - 1,type);
		return res;
	}
	res = find_pages(size_inpages,0,bmm.last_alloc_end,align >> PAGE_SHIFT);
	if (res != 0)
	{
		insert_mminfo(&bmm,(unsigned int)res,(unsigned int)res + size - 1,type);
		return res;
	}
	return 0;
}

void bootmap_info(unsigned char *msg)
{
	unsigned int index;
	kernel_printf("%s :\n", msg);
	for(index = 0;index < bmm.cnt_infos;index++)
		kernel_printf("\t%x-%x : %s\n",bmm.info[index].start,bmm.info[index].end,mem_msg[bmm.info[index].type]);
}
