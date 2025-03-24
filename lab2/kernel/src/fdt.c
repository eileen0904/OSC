#include "fdt.h"
#include "mini_uart.h"
#include "func.h"

char *cpio_file; // Device Tree Base Address
void print_tab(int level) {
    while (level--){
        uart_printf("\t");
    }
}

void dump(char *start, int len) {
    for(int i = 0; i < len; i++) {
        if (start[i] >= 0x20 && start[i] <= 0x7e)
            uart_send(start[i]);
        else
            uart_send_hex(start[i]);
    }
}

char *ALIGN(uint64_t ptr, int x) {
    if(ptr & 3) 
        ptr = (ptr + (4 - (ptr & 3)));
    
    return (char *)ptr;
}

uint32_t print_dtb(int type, char *name, char *data, uint32_t size) {
    static int tb = 0;
    if(type == FDT_BEGIN_NODE) {
        print_tab(tb);
        uart_printf("[*] Node: %s\r\n", name);
        tb++;
    }
    else if(type == FDT_END_NODE) {
        tb--;
        print_tab(tb);
        uart_send_string("[*] Node: end\r\n");
    }
    else if(type == FDT_PROP) {
        print_tab(tb);
        uart_printf("[*] %s: ", name);
        dump(data, size);
        uart_send_string("\r\n");
    }
    else {
        uart_send_string("[*] END!\r\n");
    }

    return 0;
}

uint32_t get_initramfs_addr(int type, char *name, char *data, uint32_t size) {
    if(type == FDT_PROP && !strcmp(name, "linux,initrd-start")) {
        cpio_file = (char *)(uintptr_t)fdt32_ld((void *)data);
        uart_printf("initramfs_addr: %x\r\n", cpio_file);
    }
    return 0;
}

uint32_t parse_dtb(fdt_callback cb, char *dt_struct, char *dt_strings) {
    char *cur = dt_struct;

    while(1) {
        cur = (char *)ALIGN((uint64_t)cur, 4); // Align to 4 bytes
        struct fdt_node_header *node_header = (struct fdt_node_header *)cur;
        uint32_t tag = fdtn_tag(node_header), t = 0;

        if(tag == FDT_BEGIN_NODE) {
            t = cb(FDT_BEGIN_NODE, "", node_header->name, -1);
            cur += sizeof(struct fdt_node_header) + strlen(node_header->name) + 1;
        }
        else if(tag == FDT_END_NODE) {
            t = cb(FDT_END_NODE, "", "", -1);
            cur += sizeof(struct fdt_node_header);
        }
        else if(tag == FDT_PROP) {
            struct fdt_property *prop = (struct fdt_property *)node_header;
            t = cb(FDT_PROP, dt_strings + fdtp_nameoff(prop), prop->data, fdtp_len(prop));
            cur += sizeof(struct fdt_property);
            cur += fdtp_len(prop);
        }
        else if(tag == FDT_NOP) {
            t = cb(FDT_NOP, "", "", -1);
            cur += sizeof(struct fdt_node_header);
        }
        else {
            t = cb(FDT_END, "", "", -1);
            break;
        }
        if(t != 0)
            return t;
    }
    return 0;
}

uint32_t fdt_traverse(fdt_callback cb, char *dtb) {
    struct fdt_header *header = (struct fdt_header *)dtb;

    if(fdt_magic(header) != FDT_MAGIC) {
        uart_send_string("Invalid FDT magic\n");
        return;
    }

    char *dt_struct = dtb + fdt_off_dt_struct(header);
    char *dt_strings = dtb + fdt_off_dt_strings(header);

    uint32_t r = parse_dtb(cb, dt_struct, dt_strings);

    return r;
}