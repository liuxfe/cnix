OUTPUT_FORMAT("elf64-x86-64","elf64-x86-64","elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(startup16)

PHYOFF = 0xFFFF800000000000;

SECTIONS
{
	. = PHYOFF + 0x1000;
	.text :
	{
		_text = .;
		*(.head);
		*(.init);
		*(.text);
	}

	.rodata :
	{
		*(.rdata);
		*(.rodata);
		*(.rodata.*);
	}

	. = ALIGN(0x1000);
	.data :
	{
		_data = .;
		*(.data);
	}

	. = ALIGN(0x100000);
	.bss :
	{
		_bss = .;
		pml4e = .;   . += 4096;
		pdpe0 = .;   . += 4096;
		pde0  = .;   . += 4096;
		pte0  = .;   . += 4096;
		gdt_tab = .; . += 4096;
		idt_tab = .; . += 4096;
		*(.bss);
	}
	. = ALIGN(32);
	_end = .;

	. = ALIGN(8192);
	_brk = .;

	/DISCARD/ :
	{
		*(.eh_frame);
		*(.note.GNU-stack);
		*(.comment);
	}
}
