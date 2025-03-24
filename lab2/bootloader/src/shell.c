#include "shell.h"
#include "func.h"
#include "mailbox.h"
#include "mini_uart.h"
#include "reboot.h"

void input(char *cmd) {
    char c;
    int idx = 0;

    while((c = uart_recv()) != '\n') {
        cmd[idx++] = c;
        uart_send(c);
    }
    cmd[idx] = '\0';
}

void parse_cmd(char *cmd, char *argv) {
    char *buffer = cmd;
    while(1) {
        if(*buffer == '\0') {
            argv = buffer;
            break;
        }
        if(*buffer == ' ') {
            *buffer = '\0';
            argv = buffer + 1;
            break;
        }
        buffer++;
    }
}

void shell(char *fdt) {
    uart_send_string("# ");

    char cmd[100], *argv;

    input(cmd);
    char *buffer = cmd;
    while(1) {
        if(*buffer == '\0') {
            argv = buffer;
            break;
        }
        if(*buffer == ' ') {
            *buffer = '\0';
            argv = buffer + 1;
            break;
        }
        buffer++;
    }

    uart_send_string("\r\n");

    if(strcmp(cmd, "help") == 0) {
        do_help();
    } else if(strcmp(cmd, "hello") == 0) {
        do_hello();
    } else if(strcmp(cmd, "mailbox") == 0) {
        do_mailbox();
    } else if(strcmp(cmd, "reboot") == 0) {
        reset(10);
    } else {
        uart_send_string("Unknown command\r\n");
    }
}

void do_help() {
    uart_send_string("help    : print this help menu\r\n");
    uart_send_string("hello   : print Hello World!\r\n");
    uart_send_string("mailbox : print hardware's information\r\n");
}

void do_hello() { uart_send_string("Hello World!\r\n"); }

void do_mailbox() {
    get_board_revision();
    get_arm_memory();
}