#include "_cpio.h"
#include "mini_uart.h"
#include "shell.h"
#include "fdt.h"
#include "exception.h"
#include "timer.h"

char *dtb_ptr;

void main(char *fdt) {
    uart_init();
    dtb_ptr = fdt;
    fdt_traverse(get_initramfs_addr, dtb_ptr);

    irqtask_list_init();
    timer_list_init();

    uart_interrupt_enable();
    el1_interrupt_enable(); // enable interrupt in EL1 -> EL1
    core_timer_enable();

    uart_puts("====Welcome to OSC Lab3!====\r\n");

    while(1) {
        shell(fdt);
    }
}
