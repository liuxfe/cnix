#ifndef _CNIX_DESC_H
#define _CNIX_DESC_H

union IDTdesc64 {
    struct {
	int64_t		low;
	int64_t		high;
    };
    struct {
	long		offset0_15 : 16;
	long		selector   : 16;
	long		ist        : 3;
	long		resvered1  : 5;
	long		attr       : 8;
	long		offset16_31: 16;
	long		offset32_63: 32;
	long		resvered2  : 32;
    };
};
extern union IDTdesc64 idt_tab[256];
_Static_assert(sizeof(idt_tab) == 4096, "IDT table size error");

static inline void _set_idt64(long nr, long addr, long attr)
{
	union IDTdesc64 * idt = idt_tab + nr;

	idt->high = 0x00000000ffff8000;
	idt->low  = 0x00008E0000080000;

	idt->offset0_15 = addr & 0xffff;
	idt->offset16_31 = (addr >> 16) & 0xffff;
	idt->attr = attr;
}

#define set_trap_gate(nr, addr) _set_idt64(nr, addr, 0x8F)
#define set_intr_gate(nr, addr) _set_idt64(nr, addr, 0x8E)
#define set_call_gate(nr, addr) _set_idt64(nr, addr, 0xEF)

union GDTdesc {
    int64_t		value;
    struct {
	long		limit0_15 : 16;
	long		base0_23  : 24;
	long		attr1     : 8;
	long		limit16_19: 4;
	long		attr2     : 4;
	long		base24_31 : 8;
    };
};
_Static_assert(sizeof(union GDTdesc) == 8, "GDT desc size error");

union TSSdesc {
    struct {
        int64_t low;
        int64_t high;
    };
    struct {
        long    limit0_15 : 16;
        long    base0_23  : 24;
        long    attr1     : 8;
        long    limit16_19: 4;
        long    attr2     : 4;
        long    base24_31 : 8;
        long    base32_63 : 32;
        long    resvered  : 32;
    };
};
_Static_assert(sizeof(union TSSdesc) == 16, "TSSdesc size error");

#define	NR_TSS		((4096 - 6 * 8)/16)
extern struct {
        union GDTdesc   gdt[6];
        union TSSdesc   tss[NR_TSS];
} gdt_tab;
_Static_assert(sizeof(gdt_tab) == 4096, "GDT table size error");

#endif
