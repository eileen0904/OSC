.section ".text.boot"
.global _start
.equ LOW_MEMORY, (2 * 0x200000)  // 2 * SECTION_SIZE = 4MB

_start:
    // read MPIDR_EL1 into X0
    // mpdir_el1 register tag CPU core
    mrs x0, mpidr_el1
    // check processor id
    and x0, x0, #0xFF
    // if x0 is 0 -> jump to master
    cbz x0, master
    // Hang for all non-primary CPU
    b proc_hang

proc_hang:
    b proc_hang

master:
    // Generate a register-relative address in the destination register, for a label defined in a storage map.
    adr x0, bss_begin
    adr x1, bss_end
    // x1 = x1(bss_end) - x0(bss_begin) => get size of bss segment
    sub x1, x1, x0
    bl memzero
    // initialize stack pointer
    mov sp, #LOW_MEMORY // LOW_MEMORY = 4MB
    bl main         

