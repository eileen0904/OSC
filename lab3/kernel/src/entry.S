#define CORE0_TIMER_IRQ_CTRL 0x40000040

// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 9
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
	// empty 8 bytes

	mrs x0, SPSR_EL1
	mrs x1, ELR_EL1
	stp x0, x1, [sp, 16 * 16]
	// nested

.endm

// load general registers from stack
.macro load_all
	ldp x0, x1,  [sp, 16 * 16]
	msr SPSR_EL1, x0
	msr ELR_EL1,  x1
    
	ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]

    add sp, sp, 32 * 9 
.endm

exception_handler:
    save_all
    bl exception_entry
    load_all
    eret

irq_handler:
    save_all
    bl irq_current_el_spx
    load_all
    eret

undefined_exception:
	bl c_undefined_exception

.align 11 // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
    // Exception from the current EL while using SP_EL0
    b undefined_exception 
    .align 7
    b undefined_exception 
    .align 7
    b undefined_exception
    .align 7
    b undefined_exception
    .align 7

    // Exception from the current EL while using SP_ELx
    b undefined_exception 
    .align 7
    b irq_handler
    .align 7
    b undefined_exception 
    .align 7
    b undefined_exception 
    .align 7

    b exception_handler
    .align 7
    b undefined_exception
    .align 7
    b undefined_exception
    .align 7
    b undefined_exception
    .align 7

    b undefined_exception 
    .align 7
    b undefined_exception 
    .align 7
    b undefined_exception
    .align 7
    b undefined_exception
    .align 7

.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0
    // mrs x0, cntfrq_el0
    mov x0, 0 
    msr cntp_tval_el0, x0
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1]
    ret

core_timer_handler:
    save_all
    bl core_timer_entry 
    load_all
    eret

.global to_el0_with_timer
to_el0_with_timer:
	mov x29, lr
	bl from_el1_to_el0
	bl core_timer_enable
	b proc_hang
	mov lr, x29
	ret

from_el1_to_el0:
    mov x0, 0x340 
    msr spsr_el1, x0
    ldr x0, =0x100000
    msr sp_el0, x0
    msr elr_el1, lr
    eret