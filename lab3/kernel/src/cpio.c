#include "_cpio.h"
#include "func.h"
#include "mini_uart.h"
#include <stdint.h>

unsigned int hex_to_int(const char *hex, int len) {
    unsigned int result = 0;

    for(int i = 0; i < len; i++) {
        char c = hex[i];
        result <<= 4;
        if(c >= '0' && c <= '9') {
            result |= (c - '0');
        } else if(c >= 'A' && c <= 'F') {
            result |= (c - 'A' + 10);
        } else if(c >= 'a' && c <= 'f') {
            result |= (c - 'a' + 10);
        }
    }
    return result;
}

extern char *cpio_file; // = (char *)0x8000000;
char buf[1024];
#define USTACK_SIZE 0x10000

/* header -> file name -> file data */
void cpio_parse_file(int flag, char *file) {
    char *cpio_start = cpio_file;
    struct cpio_newc_header *header = (struct cpio_newc_header *)cpio_start;

    strncpy(buf, header->c_magic, 0, 6);

    if(strcmp(buf, CPIO_NEWC_MAGIC)) {
        uart_puts("cpio magic number error\r\n");
    }

    while(!strncmp(buf, CPIO_NEWC_MAGIC, 6)) { // same:0

        unsigned int file_name_length = hex_to_int(header->c_namesize, 8);
        unsigned int file_size = hex_to_int(header->c_filesize, 8);

        // end of header is the file name
        char *file_name = (char *)(header + 1);
        char *file_data = file_name + file_name_length;

        file_data = (char *)((uintptr_t)file_data + ((4 - ((uintptr_t)file_data & 3)) & 3)); // Align to next 4-byte boundary

        strncpy(buf, file_name, 0, file_name_length);
        if(!strcmp(buf, "TRAILER!!!"))
            break;

        if(flag == 1) { // ls
            uart_puts("%s\r\n", file_name);
        } 
        else if(flag == 2) { // cat
            if(!strcmp(buf, file)) {
                strncpy(buf, file_data, 0, file_size);
                uart_puts("%s\r\n", buf);
            }
            strncpy(buf, file_name, 0, file_name_length);
        }
        else if(flag == 3) { // exec
            if(!strcmp(buf, file)) {
                char *ustack = simple_alloc(USTACK_SIZE);
                asm("msr elr_el1, %0\n\t"   // elr_el1: Set the address to return to: c_filedata
                    "msr spsr_el1, xzr\n\t" // enable interrupt (PSTATE.DAIF) -> spsr_el1[9:6]=4b0. In Basic#1 sample, EL1 interrupt is disabled.
                    "msr sp_el0, %1\n\t"    // user program stack pointer set to new stack.
                    "eret\n\t"              // Perform exception return. EL1 -> EL0
                    ::"r"(file_data),
                    "r"(ustack + USTACK_SIZE)
                );
            }
            strncpy(buf, file_name, 0, file_name_length);
        }

        // Move to the next header, aligning as necessary
        header = (struct cpio_newc_header *)(file_data + file_size); // header now point to file name
        header = (struct cpio_newc_header *)((uintptr_t)header + ((4 - ((uintptr_t)header & 3)) & 3));

        strncpy(buf, header->c_magic, 0, 6);
    }
}