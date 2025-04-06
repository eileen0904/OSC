.section ".text.boot"
.globl _start

_start:
    mov x19, x0

	mrs	x0, mpidr_el1		
	and	x0, x0, #0xFF
	cbz	x0, master
	b	proc_hang

.global proc_hang
proc_hang: 
	b 	proc_hang

master:
	bl from_el2_to_el1 // the next instruction runs in EL1
    
	ldr x1, =0x90000
	mov sp, x1

	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero
	
	ldr x1, =0x90000	
	mov sp, x1

	bl set_exception_vector_table

	bl core_timer_enable

	msr daifclr, 0xf 	// enable el1 interrupt
    
    mov x0, x19         // get fdt back

	bl  main

from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0 // 將 x0 的值寫入 hcr_el2
    mov x0, 0x3c5 // set mode with EL1h (SPSel = 1) and disable interrupt
    msr spsr_el2, x0 // spsr_el2 = x0
    msr elr_el2, lr // ELR_EL2: EL2 返回 EL1 時的 pc, lr: EL1 執行的起點
    eret // return to EL1

set_exception_vector_table:
	adr x0, exception_vector_table // vbar_el1: Vector Base Address Register (EL1)
    msr vbar_el1, x0               // Holds the exception base address for any exception that is taken to EL1.
	ret

