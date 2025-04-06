#include "irq.h"
#include "mini_uart.h"
#include "utils.h"

void irq(int d) {
    if(d) {
        asm volatile("msr DAIFclr, 0xf"); // enable IRQ
    } 
    else {
        asm volatile("msr DAIFset, 0xf"); // disable IRQ
    }
}

void change_read_irq(int d) {
    if(d) {
        // enable RX interrupt (set bit[0] = 1)
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 0x1);
    } 
    else {
        // disable RX interrupt (clear bit[0])
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(0x1));
    }
}

void change_write_irq(int d) {
    if(d) {
        // enable TX interrupt (set bit[1] = 1)
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 0x2);
    } 
    else {
        // disable TX interrupt (clear bit[1])
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(0x2));
    }
}