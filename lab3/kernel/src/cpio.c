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
struct c_file f;

/* header -> file name -> file data */
void cpio_parse_file() {
    char *cpio_start = cpio_file;
    struct cpio_newc_header *header = (struct cpio_newc_header *)cpio_start;

    strncpy(buf, header->c_magic, 0, 6);

    if(strcmp(buf, CPIO_NEWC_MAGIC)) {
        uart_send_string("cpio magic number error\r\n");
    }

    int name_idx = 0, data_idx = 0, n = 0;

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

        f.names_pos[n] = name_idx;
        for(int i = 0; i < file_name_length; i++) {
            f.names[name_idx++] = file_name[i];
        }

        f.datas_pos[n] = data_idx;
        for(int i = 0; i < file_size; i++) {
            f.datas[data_idx++] = file_data[i];
        }
        n++;
        
        // else { // cat
        //     if(!strcmp(buf, file)) {
        //         strncpy(buf, file_data, 0, file_size);
        //         uart_send_string(buf);
        //         uart_send_string("\r\n");
        //     }
        //     strncpy(buf, file_name, 0, file_name_length);
        // }

        // Move to the next header, aligning as necessary
        header = (struct cpio_newc_header *)(file_data + file_size); // header now point to file name
        header = (struct cpio_newc_header *)((uintptr_t)header + ((4 - ((uintptr_t)header & 3)) & 3));

        strncpy(buf, header->c_magic, 0, 6);
    }

    f.names_pos[n] = name_idx;
    f.datas_pos[n] = data_idx;
    f.n = n;
}

void cpio_ls() {
    uart_printf("There're %d files:\n", f.n);

    for(int i = 0; i < f.n; i++) {
        strncpy(buf, f.names, f.names_pos[i], f.names_pos[i + 1] - 1);
        uart_printf("%s\r\n", buf);
    }
}

void cpio_cat(char *file) {
    for(int i = 0; i < f.n; i++) {
        strncpy(buf, f.names, f.names_pos[i], f.names_pos[i + 1] - 1);
        if(!strcmp(buf, file)) {
            strncpy(buf, f.datas, f.datas_pos[i], f.datas_pos[i + 1] - 1);
            for(int j = f.datas_pos[i]; j < f.datas_pos[i + 1]; j++) {
                uart_send(f.datas[j]);
            }
        }
    }
}

void cpio_load(char *str) {
    for(int i = 0; i < f.n; i++) {
        strncpy(buf, f.names, f.names_pos[i], f.names_pos[i + 1] - 1);
        if(!strcmp(buf, str)) {
            void *pos = &f.datas[f.datas_pos[i]];
            uart_printf("Running code from %x...\n", pos);
            asm volatile("mov x1, 0x3c0;"
                        "msr spsr_el1, x1;"
                        "mov x1, %[var1];"
                        "ldr x2, =0x1000000;"
                        "msr elr_el1, x1;"
                        "msr sp_el0, x2;"
                        "eret"
                        :
                        : [var1] "r"(pos)
                        : "x1", "x2");
        }
    }
}