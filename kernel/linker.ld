ENTRY(kernel_entry)
SECTIONS {
	. = 1M;
	.multiboot ALIGN(4) : { *(.multiboot) }
	.text : { *(.text*) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : { *(.bss) *(COMMON) }
}
