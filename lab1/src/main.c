#include "mini_uart.h"
#include "shell.h"

int main() {
    uart_init();
    uart_send_string("Welcome to OSC Lab1!\r\n");

    while(1) {
        shell();
    }
}