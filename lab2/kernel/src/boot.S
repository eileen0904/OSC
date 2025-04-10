.section ".text.boot"
.globl _start

_start:
    mov x19, x0

	mrs	x0, mpidr_el1		
	and	x0, x0, #0xFF
	cbz	x0, master
	b	proc_hang

proc_hang: 
	b 	proc_hang

master:
	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero
	
	ldr x1, =0x90000	
	mov sp, x1
    
    mov x0, x19         // get fdt back

	bl  main
	b 	proc_hang