#include "cpio.h"
#include "mini_uart.h"
#include "shell.h"
#include "fdt.h"

void main(char *fdt) {
    uart_init();
    fdt_traverse(get_initramfs_addr, fdt);
    uart_send_string("====Welcome to OSC Lab2!====\r\n");

    while(1) {
        shell(fdt);
    }
}
