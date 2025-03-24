_bootloader = 0x60000;
_kernel = 0x80000;

SECTIONS
{
	. = _bootloader;
	_start = .;
	.text.boot : { *(.text.boot) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	. = ALIGN(0x8);
	.bss : {
	bss_begin = .;
	 *(.bss) 
	bss_end = .;
	} 
	_end = .;
}

_loader_size = (_end - _start) >> 3;