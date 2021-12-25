OUTPUT_FORMAT("elf64-x86-64","elf64-x86-64","elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

PHYOFF = 0xFFFF800000000000;

SECTIONS
{
	. = PHYOFF + 0x1000;
	.text :
	{
		_text = .;
		*(.head);
		*(.text);
	}

	. = ALIGN(8);
	.data :
	{
		_data = .;
		*(.data);
		*(.rdata);
		*(.rodata);
		*(.rodata.*);
	}

	. = ALIGN(0x180000);
	.bss :
	{
		_bss = .;
		*(.bss);
	}
	. = ALIGN(8);
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
