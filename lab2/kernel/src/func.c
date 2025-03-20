#include "func.h"
#include "mini_uart.h"

int strcmp(const char *s1, const char *s2) {
    while(*s1 == *s2) {
        if(*s1 == '\0') 
            return 0;
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, int n) {
    while(*s1 == *s2 && n > 0) {
        if(*s1 == '\0') 
            return 0;
        s1++;
        s2++;
        n--;
    }
    return *s1 - *s2;
}

int strlen(const char *s) {
    int len = 0;
    while(*s != '\0') {
        len++;
        s++;
    }
    return len;
}

void strncpy(char *dest, const char *src, int start, int n) {
    for(int i = start; i < n; i++) {
        dest[i] = src[i];
    }
    dest[n] = '\0';
}

extern char _memory[]; // boot.S
char *mem_ptr = _memory;

void *simple_alloc(unsigned int size) {
    mem_ptr += size;
    if(mem_ptr >= 0x20000000) {
        uart_send_string("Out of memory\n");
        mem_ptr -= size;
        return 0;
    }
    return (void *)(mem_ptr - size);
}