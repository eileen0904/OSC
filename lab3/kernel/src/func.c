#include "func.h"
#include "mini_uart.h"
#include <stdint.h>

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

extern char _heap_top;
static char *htop_ptr = &_heap_top;

void *simple_alloc(unsigned int size) {
    // -> htop_ptr
    // htop_ptr + 0x02:  heap_block size
    // htop_ptr + 0x10 ~ htop_ptr + 0x10 * k:
    //            { heap_block }
    // -> htop_ptr

    // 0x10 for heap_block header
    char *r = htop_ptr + 0x10;
    // size paddling to multiple of 0x10
    size = 0x10 + size - size % 0x10;
    *(unsigned int *)(r - 0x8) = size;
    htop_ptr += size;
    return r;
}

static char *int_to_str(char *buf, int num, int base, int is_signed) {
    char tmp[32];
    char *digits = "0123456789abcdef";
    int i = 0;
    int negative = 0;

    if(is_signed && num < 0) {
        negative = 1;
        num = -num;
    }

    // 整數轉字串（反向）
    do {
        tmp[i++] = digits[num % base];
        num /= base;
    } while(num != 0);

    if(negative)
        tmp[i++] = '-';

    // 反轉過來寫入 buf
    while(i--)
        *buf++ = tmp[i];

    return buf;
}

int vsprintf(char *buf, const char *fmt, __builtin_va_list args) {
    char *str = buf;
    const char *s;

    for(; *fmt; fmt++) {
        if(*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        fmt++; // skip '%'

        switch(*fmt) {
        case 'c': {
            char c = (char)__builtin_va_arg(args, int);
            *str++ = c;
            break;
        }
        case 's': {
            s = __builtin_va_arg(args, char *);
            while(*s)
                *str++ = *s++;
            break;
        }
        case 'd': {
            int val = __builtin_va_arg(args, int);
            str = int_to_str(str, val, 10, 1);
            break;
        }
        case 'x': {
            int val = __builtin_va_arg(args, int);
            str = int_to_str(str, val, 16, 0);
            break;
        }
        default:
            *str++ = '%';
            *str++ = *fmt;
            break;
        }
    }

    *str = '\0'; // 結尾字元
    return str - buf;
}

int str_to_int(char *str) {
    int res = 0;

    for(int i = 0; str[i] != '\0'; i++) {
        if(str[i] > '9' || str[i] < '0')
            return res;
        res = res * 10 + str[i] - '0';
    }

    return res;
}
