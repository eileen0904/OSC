#include "shell.h"
#include "_cpio.h"
#include "fdt.h"
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
    while (1) {
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
    }
    else if(strcmp(cmd, "hello") == 0) {
        do_hello();
    }
    else if(strcmp(cmd, "mailbox") == 0) {
        do_mailbox();
    }
    else if(strcmp(cmd, "ls") == 0) {
        do_ls();
    }
    else if(strcmp(cmd, "cat") == 0) {
        do_cat(argv);
    }
    else if(strcmp(cmd, "memAlloc") == 0) {
        do_memAlloc();
    }
    else if(strcmp(cmd, "get_initramd") == 0) {
        do_get_initramd(fdt);
    }
    else if(strcmp(cmd, "dtb") == 0) {
        do_dtb(fdt);
    } else if(strcmp(cmd, "reboot") == 0) {
        reset(10);
    } 
    else {
        uart_send_string("Unknown command\r\n");
    }
}

void do_help() {
    uart_send_string("help           : print this help menu\r\n");
    uart_send_string("hello          : print Hello World!\r\n");
    uart_send_string("mailbox        : print hardware's information\r\n");
    uart_send_string("ls             : list file\r\n");
    uart_send_string("cat <filename> : print file\r\n");
    uart_send_string("memAlloc       : test the allocator\r\n");
    uart_send_string("get_initramd   : get initial ramdisk\r\n");
    uart_send_string("dtb            : print the device tree\r\n");
    uart_send_string("reboot         : reboot the board\r\n");
}

void do_hello() {
    uart_send_string("Hello World!\r\n");
}

void do_mailbox() {
    get_board_revision();
    get_arm_memory();
}

void do_ls() {
    cpio_parse_file(1, "NO_FILE");
}

void do_cat(char *argv) {
    uart_send_string("Filename: ");
    uart_send_string(argv);
    uart_send_string("\r\n");
    cpio_parse_file(0, argv);
}

void do_memAlloc() {
    char *str = simple_alloc(8);
    char *str2 = simple_alloc(10);

    for(int i = 0; i < 7; i++) str[i] = 'a';
    str[7] = '\0';
    for(int i = 0; i < 9; i++) str2[i] = 'b';
    str2[9] = '\0';

    uart_send_string(str);
    uart_send_string("\r\n");
    uart_send_string(str2);
    uart_send_string("\r\n");
}

void do_get_initramd(char *fdt) { // use devicetree to get initial ramdisk
    fdt_traverse(get_initramfs_addr, fdt);
}

void do_dtb(char *fdt) { // print the device tree
    fdt_traverse(print_dtb, fdt);
}