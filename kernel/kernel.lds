OUTPUT_FORMAT("elf64-x86-64","elf64-x86-64","elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

SECTIONS
{
	. = 0x1000;
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

	. = ALIGN(0x100000);
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
