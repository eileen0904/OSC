SECTIONS
{
    . = 0x80000;
    .text.boot : {KEEP(*(.text.boot))}
    .text : {*(.text)}

    .rodata : {*(.rodata)}
    .data : {*(.data)}

    /* record start/end of bss to fill 0 */

    . = ALIGN(8);
    PROVIDE(bss_begin = .);
    .bss : { *(.bss*) }
    PROVIDE(bss_end = .);
    PROVIDE(_end = .);
}