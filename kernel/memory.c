#include <cnix/kernel.h>
#include <cnix/asm.h>
#include <cnix/desc.h>
#include <cnix/traps.h>
#include <cnix/spinlock.h>

struct E820_struct {
    uint64_t  addr;
    uint64_t  len;
    int32_t   type;
} __attribute__((packed)) ;

#if 0
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
#endif

#define NR_ZONE_MAX	32
struct mem_zone{
	uint64_t start;
	uint64_t end;
	uint64_t free;
} mem_zone[NR_ZONE_MAX] = { 0 };
int NR_ZONE = 0;

static void __init setup_zone()
{
	unsigned long _mem_start = __v2p((unsigned long)&_brk) + 8192*NR_CPUS;

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

static spinlock_t mem_alloc_lock = { SPIN_UNLOCK };

long alloc_page()
{
	spin_lock(&mem_alloc_lock);
	for(int i=0; i<NR_ZONE; i++){
		if(mem_zone[i].free>=mem_zone[i].end)
			continue;
		mem_zone[i].free += 4096;
		spin_unlock(&mem_alloc_lock);
		return (mem_zone[i].free - 4096);
	}
	printk("OOM");
	return 0;
}

long alloc_2page()
{
	spin_lock(&mem_alloc_lock);
	for(int i=0; i<NR_ZONE; i++){
		if(mem_zone[i].free & (8192ULL-1))
			mem_zone[i].free += 4096;
		if(mem_zone[i].free>=mem_zone[i].end)
			continue;
		mem_zone[i].free += 8192;
		spin_unlock(&mem_alloc_lock);
		return (mem_zone[i].free - 8192);
	}
	printk("OOM");
	return 0;
}

extern struct{
	uint64_t entry[4096/8];
} pml4e, pdpe0, pde0, pde1, pde2, pde3, pde4, pde5, pde6, pde7, pte0;
#define KB * 1024
#define MB * 1024 KB
#define GB * 1024 MB

static inline void setup_pgt()
{
	int i;
	uint64_t p;

	pte0.entry[0] = 0x7 ;//| (1ULL<<63);
	for(i=1,p=0x1000; p<__v2p((uint64_t)&_data); p+=0x1000)
		pte0.entry[i++] = p + 0x07;
	for(p=__v2p((uint64_t)&_data); p<0x200000; p+=0x1000)
		pte0.entry[i++] = (p + 0x07) ;//| (1ULL <<63);

	pde0.entry[0] = __v2p((uint64_t)&pte0) + 0x07;
	for(i=1,p=2 MB; i<4096/8; i++,p+=2 MB)
		pde0.entry[i] = p + 0x87;

	uint64_t *a = (uint64_t*)&pde0 + 1;
	for(i=1,p=2 MB; i<4096/8 *8; i++, p += 2 MB)
		*a++ = p + 0x87;

	pdpe0.entry[0] = __v2p((uint64_t)&pde0) + 0x07;
	pdpe0.entry[1] = __v2p((uint64_t)&pde1) + 0x07;
	pdpe0.entry[2] = __v2p((uint64_t)&pde2) + 0x07;
	pdpe0.entry[3] = __v2p((uint64_t)&pde3) + 0x07;
	pdpe0.entry[4] = __v2p((uint64_t)&pde4) + 0x07;
	pdpe0.entry[5] = __v2p((uint64_t)&pde5) + 0x07;
	pdpe0.entry[6] = __v2p((uint64_t)&pde6) + 0x07;
	pdpe0.entry[7] = __v2p((uint64_t)&pde7) + 0x07;

	pml4e.entry[0] = 0; // clear it.
}

void __init setup_mem()
{
	setup_pgt();
	setup_zone();
}
