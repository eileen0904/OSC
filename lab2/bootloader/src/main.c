#include "mini_uart.h"
#include "bootloader.h"

void main(char *fdt) {
    uart_init();
    uart_send_string("Start bootloading\r\n");
    load_img(fdt);
}