#include "mini_uart.h"
#include "exception.h"
#include "gpio.h"
#include "irq.h"
#include "utils.h"
#include "func.h"
#include <stdarg.h>
#include <stdint.h>

#define SIGN 1

char tx_buffer[1024] = {};
unsigned int tx_widx = 0, tx_ridx = 0;
char rx_buffer[1024] = {};
unsigned int rx_widx = 0, rx_ridx = 0;

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

void uart_interrupt_enable() {
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 1); // enable read interrupt
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 2); // enable write interrupt
    put32(ENABLE_IRQS_1, get32(AUX_MU_IER_REG) | 1 << 29);
}

void uart_interrupt_disable() {
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(1)); // disable read interrupt
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(2)); // disable write interrupt
}

int uart_puts(char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    char buf[1024];

    char *str = (char *)buf;
    int count = vsprintf(str, fmt, args);

    while(*str) {
        if(*str == '\n')
            uart_async_putc('\r');
        uart_async_putc(*str++);
    }
    __builtin_va_end(args);
    return count;
}

// read from buffer
char uart_async_getc() {
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 1); // enable read interrupt

    while(rx_ridx == rx_widx)
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 1); // enable read interrupt
    
    el1_interrupt_disable();

    char r = rx_buffer[rx_ridx++];
    if(rx_ridx >= 1024) rx_ridx = 0;
    
    el1_interrupt_enable();
    return r;
}

// writes to buffer
void uart_async_putc(char c) {
    // if buffer full, wait for uart_w_irq_handler
    while((tx_widx + 1) % 1024 == tx_ridx)
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 2); // enable write interrupt
    
    el1_interrupt_disable();

    tx_buffer[tx_widx++] = c;
    if(tx_widx >= 1024) tx_widx = 0; // cycle pointer
    
    el1_interrupt_enable();
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 2); // enable write interrupt
}

// write to buffer then output
void uart_r_irq_handler() {
    if((rx_widx + 1) % 1024 == rx_ridx) {
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(1));  // disable read interrupt
        return;
    }

    rx_buffer[rx_widx++] = uart_recv();
    if(rx_widx >= 1024) rx_widx = 0;
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 1); // enable read interrupt
}

// read from buffer then output
void uart_w_irq_handler() {
    if(tx_ridx == tx_widx) {
        put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(2));  // disable write interrupt
        return;  // buffer empty
    }

    uart_send(tx_buffer[tx_ridx++]);
    if(tx_ridx >= 1024) tx_ridx = 0;
    put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) | 2); // enable write interrupt
}