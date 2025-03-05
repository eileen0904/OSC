#include "mini_uart.h"
#include "shell.h"
#include "string.h"
#include "mailbox.h"

void input(char *cmd){
    char c;
    int idx = 0;

    while ((c = uart_recv()) != '\n') {
        cmd[idx++] = c;
        uart_send(c);
    }
    cmd[idx] = '\0';
}

void shell() {
    uart_send_string("# ");

    char cmd[10];
    input(cmd);
    uart_send('\n');

    if(strcmp(cmd, "help") == 0) {
        uart_send_string("help    : print this help menu\r\n");
        uart_send_string("hello   : print Hello World!\r\n");
        uart_send_string("mailbox : print hardware's information\r\n");
    }
    else if(strcmp(cmd, "hello") == 0) {
        uart_send_string("Hello World!\r\n");
    }
    else if(strcmp(cmd, "mailbox") == 0) {
        get_board_revision();
        get_arm_memory();
    }
    else {
        uart_send_string("Unknown command\r\n");
    }
}