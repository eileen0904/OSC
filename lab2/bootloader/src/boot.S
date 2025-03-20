.section ".text.boot"
.globl _start

_start:
    mov x19, x0

	mrs	x0, mpidr_el1		
	and	x0, x0, #0xFF
	cbz	x0, master	
	b   proc_hang

proc_hang: 
	b   proc_hang

master:
	ldr x1, = _kernel
    ldr x2, = _bootloader
    ldr w3, = _loader_size

relocate:
    ldr x4, [x1], #8
    str x4, [x2], #8
    sub w3, w3, #1
    cbnz w3, relocate

	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl  memzero

	mov sp, #0x70000

    mov x0, x19
	bl 	main - 0x20000

