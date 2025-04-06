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
        cpio_ls();
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
    else if(strcmp(cmd, "load") == 0) {
        cpio_load(argv);
    }
    else if(strcmp(cmd, "async") == 0) {
        do_async_test();
    } 
    else if(strcmp(cmd, "setTimeout") == 0){
        setTimeout();
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

void do_cat(char *argv) {
    uart_send_string("Filename: ");
    uart_send_string(argv);
    uart_send_string("\r\n");
    cpio_cat(argv);
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

void do_async_test(){
    uart_irq_on();

    uart_irq_send("Test\r\n\0");

    int t = 100000000;
    while(t--);

    char *str = simple_alloc(100);
    uart_irq_read(str);

    t = 10000000;
    while(t--);

    uart_irq_off();
    uart_printf("%s, End\r\n", str);
}

void setTimeout() {
    char buf[1024];
    uart_printf("Input time(ms): ");
    input(buf);
    unsigned long t = stoi(buf);
    uart_printf("\r\n");
    uart_printf("Input data to output: ");
    input(buf);
    uart_printf("\r\n");
    set_timeout(t, buf);
}

