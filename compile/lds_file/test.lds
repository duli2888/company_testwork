SECTIONS
{
	. = 0x10000;
	.text : { *(.text) }
	. = 0x200000;
	.data : { *(.data) }
	.bss : { *(.bss) }
}
