#include "mini_uart.h"
#include "gpio.h"
#include "utils.h"
#include "irq.h"
#include <stdarg.h>
#include <stdint.h>

#define SIGN 1

char write_buffer[1024];
int write_tail = 0, write_idx = 0;
char recv_buffer[1024];
int recv_tail = 0, recv_idx = 0;

void uart_init(void) {
    /* Init UART */
    put32(AUX_ENABLES,
          1); // Enable mini uart (this also enables access to its registers)
    put32(AUX_MU_CNTL_REG, 0); // Disable auto flow control and disable receiver
                               // and transmitter (for now)
    put32(AUX_MU_IER_REG, 0);  // Disable receive and transmit interrupts
    put32(AUX_MU_LCR_REG, 3);  // Enable 8 bit mode
    put32(AUX_MU_MCR_REG, 0);  // Set RTS line to be always high
    put32(AUX_MU_BAUD_REG, 270); // Set baud rate to 115200

    /* Enable alternate function */
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7 << 12); // clear gpio14
    selector |= 2 << 12;    // set ALT5 for gpio14
    selector &= ~(7 << 15); // clear gpio15
    selector |= 2 << 15;    // set ALT5 for gpio15
    put32(GPFSEL1, selector);

    /* Disable Pull-up and Pull-down of gpio14 and gpio15 */
    put32(GPPUD, 0);
    delay(150);
    put32(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    put32(GPPUDCLK0, 0);

    put32(AUX_MU_CNTL_REG, 3); // Enable transmitter and receiver

    put32(UART_ICR, 0x7FF); /* clear interrupts */
    put32(UART_FBRD, 0xB);
    put32(UART_LCRH, (0b11 << 5)); /* 8n1 */
    put32(UART_CR, 0x301); /* enable Tx, Rx, FIFO */
    put32(UART_IMSC, (3 << 4)); /* Tx, Rx */
}

char uart_recv() {
    while(!(get32(AUX_MU_LSR_REG) & 0x01))
        ;
    char r = (char)(get32(AUX_MU_IO_REG));

    return r == '\r' ? '\r\n' : r;
}

void uart_send(char c) {
    while(!(get32(AUX_MU_LSR_REG) & 0x20))
        ;
    put32(AUX_MU_IO_REG, c);
}

void uart_send_string(char *str) {
    for(int i = 0; str[i] != '\0'; i++) {
        uart_send((char)str[i]);
    }
}

void uart_send_hex(unsigned int num) {
    char hex_string[9];
    for(int i = 7; i >= 0; i--) {
        int nibble = num & 0xF;
        hex_string[i] = nibble > 9 ? (nibble - 10) + 'A' : nibble + '0';
        num >>= 4;
    }
    hex_string[8] = '\0';
    uart_send_string(hex_string);
}

void uart_send_num(int64_t num, int base, int type) {
    const char digits[16] = "0123456789ABCDEF";
    char tmp[66];
    int i;

    if(type & SIGN) {
        if(num < 0)
            uart_send('-');
    }

    i = 0;

    if(num == 0)
        tmp[i++] = '0';
    else {
        while(num != 0) {
            uint8_t r = ((uint64_t)num) % base;
            num = ((uint64_t)num) / base;
            tmp[i++] = digits[r];
        }
    }

    while(--i >= 0)
        uart_send(tmp[i]);
}

// Ref: https://elixir.bootlin.com/linux/v3.5/source/arch/x86/boot/printf.c#L115
void uart_printf(char *fmt, ...) {
    const char *s;
    char c;
    uint64_t num;
    char width;

    va_list args;
    va_start(args, fmt);

    for(; *fmt; ++fmt) {
        if(*fmt != '%') {
            uart_send(*fmt);
            continue;
        }

        ++fmt;

        width = 0;
        if(fmt[0] == 'l' && fmt[1] == 'l') {
            width = 1;
            fmt += 2;
        }

        switch(*fmt) {
        case 'c':
            c = va_arg(args, uint32_t) & 0xff;
            uart_send(c);
            continue;
        case 'd':
            if(width)
                num = va_arg(args, int64_t);
            else
                num = va_arg(args, int32_t);

            uart_send_num(num, 10, SIGN);
            continue;
        case 's':
            s = va_arg(args, char *);
            uart_send_string((char *)s);
            continue;
        case 'x':
            if(width)
                num = va_arg(args, uint64_t);
            else
                num = va_arg(args, uint32_t);

            uart_send_num(num, 16, 0);
            continue;
        }
    }
}

void uart_irq_on() {
    change_read_irq(1);
    put32(ENABLE_IRQS_1, get32(ENABLE_IRQS_1) | (1 << 29));
}

void uart_irq_off() {
    change_read_irq(0);
    put32(DISABLE_IRQS_1, get32(DISABLE_IRQS_1) | (1 << 29));
}

void uart_irq_send(char *str) {
    for(int i = 0; str[i] != '\0'; i++) {
        write_buffer[write_tail++] = str[i];
    }
    change_write_irq(1);
}

void uart_irq_read(char *str) {
    int ind = 0;
    while(recv_idx < recv_tail) {
        str[ind++] = recv_buffer[recv_idx++];
    }
}

void write_handler() {
    irq(0);

    while(write_idx != write_tail) {
        char c = write_buffer[write_idx++];
        put32(AUX_MU_IO_REG, (unsigned int)c);
    }

    irq(1);
}

void recv_handler() {
    irq(0);

    char c = (char)get32(AUX_MU_IO_REG);
    if(c != 0) {
        recv_buffer[recv_tail++] = c;
    }
    change_read_irq(1);

    irq(1);
}