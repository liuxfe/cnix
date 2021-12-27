#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/desc.h>
#include <cnix/traps.h>

struct E820_struct {
    uint64_t  addr;
    uint64_t  len;
    int32_t   type;
} __attribute__((packed)) ;

static char* mem_type_str[] = {
    "",
    "RAM",
    "ROM or Reserved",
    "ACPI Reclaim Memory",
    "ACPI NVS Memory",
    "Undefine"
};

static void dump_e820()
{
	struct E820_struct *E820 = (struct E820_struct*)__p2v(0x500);
	while(E820->type) {
		printk("  %#018x-%#018x, %s\n", E820->addr,
			E820->addr + E820->len,
			mem_type_str[E820->type]);
		E820++;
	}
}

#define NR_ZONE_MAX	32
struct mem_zone{
	uint64_t start;
	uint64_t end;
	uint64_t free;
} mem_zone[NR_ZONE_MAX];
int NR_ZONE;
extern long _data, _brk;
extern int NR_CPUS;
void mem_init()
{
	dump_e820();
	long _mem_start = (long)&_brk + 8192*NR_CPUS - PHYOFF;

	struct E820_struct *E820 = (struct E820_struct*)__p2v(0x500);

	int i;
	for(i=0; i<NR_ZONE_MAX && E820->type; E820++){
		if(E820->type != 1)
			continue;
		if(E820->addr +E820->len < _mem_start)
			continue;
		if(E820->addr < _mem_start){
			mem_zone[i].start = _mem_start;
			mem_zone[i].end = E820->addr + E820->len;
			if(mem_zone[i].end > 8 * 1024 * 1024 * 1024ULL)
				mem_zone[i].end = 8 * 1024 * 1024 * 1024ULL;

			mem_zone[i].free = mem_zone[i].start;
			i++;
		}else{
			if(E820->addr > 8 * 1024 * 1024 * 1024ULL)
				break;
			mem_zone[i].start = E820->addr;
			mem_zone[i].end = E820->addr + E820->len;
			if(mem_zone[i].end > 8 * 1024 * 1024 * 1024ULL)
				mem_zone[i].end = 8 * 1024 * 1024 * 1024ULL;

			mem_zone[i].free = mem_zone[i].start;
			i++;
		}
	}
	NR_ZONE = i;

	printk("Useable memory:\n");
	for(i=0; i<NR_ZONE;i++)
		printk("  %0#18x-%0#18x\n", mem_zone[i].start, mem_zone[i].end);
}

long alloc_page()
{
	for(int i=0; i<NR_ZONE; i++){
		if(mem_zone[i].free>=mem_zone[i].end)
			continue;
		mem_zone[i].free += 4096;
		return (mem_zone[i].free - 4096);
	}
	printk("OOM");
	return NULL;
}

long alloc_2page()
{
	for(int i=0; i<NR_ZONE; i++){
		if(mem_zone[i].free & (8192ULL-1))
			mem_zone[i].free += 4096;
		if(mem_zone[i].free>=mem_zone[i].end)
			continue;
		mem_zone[i].free += 8192;
		return (mem_zone[i].free - 8192);
	}
	printk("OOM");
	return NULL;
}
