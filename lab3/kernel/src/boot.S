.section ".text.boot"

.global _start

_start:
    bl from_el2_to_el1

set_exception_vector_table:
    adr x1, exception_vector_table // vbar_el1: Vector Base Address Register (EL1)
    msr vbar_el1, x1               // Holds the exception base address for any exception that is taken to EL1.

setup_stack:
    ldr     x1, =_stack_top
    mov     sp, x1                 // sp = x1

setup_bss:
    ldr     x1, =_bss_top          // load 32bits to x1 from _bss_top
    ldr     w2, =_bss_size         // load 64bits to w2 from _bss_size

init_bss:
    cbz     w2, run_main
    str     xzr, [x1], #8          // store xzr to addr[x1], then x1=x1+8
    sub     w2, w2, #1
    cbnz    w2, init_bss

run_main:
    ldr     x1, =dtb_ptr
    str     x0, [x1], #8
    bl      main                   

proc_hang:
    wfe                            
    b       proc_hang

from_el2_to_el1:
    mov x1, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x1 // 將 x1 的值寫入 hcr_el2
    mov x1, 0x3c5 // set mode with EL1h (SPSel = 1) and disable interrupt
    msr spsr_el2, x1 // spsr_el2 = x0
    msr elr_el2, lr // ELR_EL2: EL2 返回 EL1 時的 pc, lr: EL1 執行的起點
    eret // return to EL1