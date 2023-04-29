SECTIONS
{
	. = 0x7C00;
	.mbr16 : { *(.mbr16) }
	. = 0x8000;
	.entry16 : { *(.entry16) }
	.entry32 : { *(.entry32) }
	.text : { *(.text) }
	.rodata : { *(.rodata*) }

	.init_array :
	{
		BUILD/crti.o(.init_array)
		KEEP (*(SORT(EXCLUDE_FILE(BUILD/crti.o BUILD/crtn.o) .init_array)))
		KEEP (*(EXCLUDE_FILE(BUILD/crti.o BUILD/crtn.o) .init_array))
		BUILD/crtn.o(.init_array)
	}

	.fini_array :
	{
		BUILD/crti.o(.fini_array)
		KEEP (*(SORT(EXCLUDE_FILE(BUILD/crti.o BUILD/crtn.o) .fini_array.*)))
		KEEP (*(EXCLUDE_FILE(BUILD/crti.o BUILD/crtn.o) .fini_array))
		BUILD/crtn.o(.fini_array)
	}

	.data : { *(.data) }
	.bss : { *(.bss) }
}
