#include "shell.h"
#include "_cpio.h"
#include "fdt.h"
#include "func.h"
#include "mailbox.h"
#include "mini_uart.h"
#include "reboot.h"
#include "timer.h"

void input(char *cmd) {
    char c;
    int idx = 0;

    for(int i = 0; i < 100; i++) {
        cmd[i] = '\0';
    }

    while((c = uart_async_getc()) != '\n') {
        cmd[idx++] = c;
        uart_async_putc(c);
    }
    cmd[idx] = '\0';
}

char *parse_cmd(char *cmd) {
    char *argv;
    while(1) {
        if(*cmd == '\0') {
            argv = cmd;
            break;
        }
        if(*cmd == ' ') {
            *cmd = '\0';
            argv = cmd + 1;
            break;
        }
        cmd++;
    }
    return argv;
}

void shell(char *fdt) {
    uart_puts("# ");

    char cmd[100];

    input(cmd);
    char *argv = parse_cmd(cmd);

    uart_puts("\r\n");

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
        cpio_parse_file(1, "NO_FILE");
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
    else if(strcmp(cmd, "exec") == 0) {
        cpio_parse_file(3, argv);
    }
    else if(strcmp(cmd, "setTimeout") == 0){
        char *sec = parse_cmd(argv);
        do_setTimeout(argv, sec);
    } 
    else if(strcmp(cmd, "set2sAlert") == 0) {
        do_set2sAlert();
    } else {
        uart_puts("Unknown command\r\n");
    }
}

void do_help() {
    uart_puts("help            : print this help menu\r\n");
    uart_puts("hello           : print Hello World!\r\n");
    uart_puts("mailbox         : print hardware's information\r\n");
    uart_puts("ls              : list file\r\n");
    uart_puts("cat <filename>  : print file\r\n");
    uart_puts("memAlloc        : test the allocator\r\n");
    uart_puts("get_initramd    : get initial ramdisk\r\n");
    uart_puts("dtb             : print the device tree\r\n");
    uart_puts("reboot          : reboot the board\r\n");
    uart_puts("exec <filename> : execute a command, replacing current image with a new image\r\n");
    uart_puts("setTimeout [MESSAGE] [SECONDS]\r\n");
    uart_puts("set2sAlert      : set core timer interrupt every 2 second\r\n");
}

void do_hello() {
    uart_puts("Hello World!\r\n");
}

void do_mailbox() {
    get_board_revision();
    get_arm_memory();
}

void do_cat(char *argv) {
    uart_puts("Filename: %s\r\n", argv);
    cpio_parse_file(2, argv);
}

void do_memAlloc() {
    char *str = simple_alloc(8);
    char *str2 = simple_alloc(10);

    for(int i = 0; i < 7; i++) str[i] = 'a';
    str[7] = '\0';
    for(int i = 0; i < 9; i++) str2[i] = 'b';
    str2[9] = '\0';

    uart_puts("%s\r\n", str);
    uart_puts("%s\r\n", str2);
}

void do_get_initramd(char *fdt) { // use devicetree to get initial ramdisk
    fdt_traverse(get_initramfs_addr, fdt);
}

void do_dtb(char *fdt) { // print the device tree
    fdt_traverse(print_dtb, fdt);
}

void do_setTimeout(char *msg, char *sec) {
    add_timer(uart_printf, str_to_int(sec), msg);
}

void do_set2sAlert() {
    add_timer(timer_set2sAlert, 2, "2sAlert");
}