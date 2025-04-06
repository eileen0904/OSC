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

char *now = 0x90000;
void *simple_alloc(unsigned int size) {
    now += size;
    return (char *)(now - size);
}

unsigned long stoi(char *str) {
    unsigned long res = 0;
    for(int i = 0; str[i] != '\0'; i++) {
        res = res * 10 + str[i] - '0';
    }
    return res;
}