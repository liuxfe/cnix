#ifndef _CNIX_DESC_H
#define _CNIX_DESC_H

union IDTdesc64 {
    struct {
	int64_t low;
	int64_t high;
    };
    struct {
	int   offset0_15 : 16;
	int   selector   : 16;
	int   ist        : 3;
	int   resvered1  : 5;
	int   attr       : 8;
	int   offset16_31: 16;
	int   offset32_63: 32;
	int   resvered2  : 32;
    };
};
extern union IDTdesc64 idt_tab[256];
_Static_assert(sizeof(idt_tab)!=8192, "IDT table size error");

union GDTdesc {
    int64_t     value;
    struct {
        int     limit0_15 : 16;
        int     base0_23  : 24;
        int     attr1     : 8;
        int     limit16_19: 4;
        int     attr2     : 4;
        int     base24_31 : 8;
    };
};
_Static_assert(sizeof(union GDTdesc)!=8, "GDT desc size error");

union TSSdesc {
    struct {
        int64_t low;
        int64_t high;
    };
    struct {
        int     limit0_15 : 16;
        int     base0_23  : 24;
        int     attr1     : 8;
        int     limit16_19: 4;
        int     attr2     : 4;
        int     base24_31 : 8;
        int     base32_63 : 32;
        int     resvered  : 32;
    };
};
_Static_assert(sizeof(union TSSdesc)!=16, "TSS desc size error");

#define	NR_TSS		((8192 - 6 * 8)/16)
extern struct {
    union GDTdesc   gdt[6];
    union TSSdesc   tss[NR_TSS];
} gdt_tab;
_Static_assert(sizeof(gdt_tab)!= 8192, "GDT table size error");


inline void _set_idt64(long nr, long addr, long attr)
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

#endif
