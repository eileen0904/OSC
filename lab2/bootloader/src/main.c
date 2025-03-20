#include "mini_uart.h"
#include "shell.h"

void main(char *fdt) {
    uart_init();
    uart_send_string("Start bootloading\r\n");
    shell(fdt);
}