SECTIONS
{
	. = 0x0;
	.text : { *(.text) }
	. = 0x0400;
	.data : { *(.data) }
	.bss : { *(.bss) }
}
