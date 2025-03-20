#include "bootloader.h"
#include "mini_uart.h"

extern char _kernel[];

typedef void (*kernel_funcp)(char *);

/* 
Receive the Kernel image from an external source via UART, 
load it into memory at address 0x80000, and then jump to execute it. 
*/
void load_img(char *fdt) {
    unsigned int size = 0;
    unsigned char *size_buffer = (unsigned char *) &size;

    unsigned int len;
    char *p = _kernel;

    uart_printf("[*] Kernel base address: %x\r\n", _kernel);

    len = uart_recv_uint();

    uart_printf("[*] Kernel image length: %d\r\n", len);

    while(len--) 
        *p++ = uart_recv();

    // Execute kernel
    ((kernel_funcp)_kernel)(fdt);
}