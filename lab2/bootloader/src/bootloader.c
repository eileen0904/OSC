#include "bootloader.h"
#include "mini_uart.h"
#include "func.h"

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

    for(int i = 0; i < 4; i++) {
        len <<= 8;
        len |= uart_recv_uint();
    }
    while(len--){
        *p = uart_recv_raw();
        uart_send(*p);
        p++;
    }
    ((kernel_funcp)_kernel)(fdt);
}