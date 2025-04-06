#ifndef _P_MINI_UART_H
#define _P_MINI_UART_H

#include "base.h"
#include <sys/types.h>
#include <stdint.h>

#define AUX_ENABLES (PBASE + 0x00215004)
#define AUX_MU_IO_REG (PBASE + 0x00215040)   // used to write data to and read data from the UART FIFOs
#define AUX_MU_IER_REG (PBASE + 0x00215044)  // 0:Disable interrupt because currently you don’t need interrupt.
#define AUX_MU_IIR_REG (PBASE + 0x00215048)  // Set AUX_MU_IIR_REG to 6. || shows the interrupt status
#define AUX_MU_LCR_REG (PBASE + 0x0021504C)  // Set the data size.
#define AUX_MU_MCR_REG (PBASE + 0x00215050)  // 0:Don’t need auto flow control.
#define AUX_MU_LSR_REG (PBASE + 0x00215054)  // shows the data status
#define AUX_MU_CNTL_REG (PBASE + 0x00215060) // transmitter and receiver
#define AUX_MU_BAUD_REG (PBASE + 0x00215068) // 270:Set baud rate to 115200

#define UART_FBRD (PBASE + 0x00201028) /* RFractional Baud Rate Divisor */
#define UART_LCRH (PBASE + 0x0020102C) /* Line Control Register */
#define UART_CR (PBASE + 0x00201030) /* Control Register */
#define UART_IMSC (PBASE + 0x00201038) /* Interupt FIFO Level Select Register */
#define UART_ICR (PBASE + 0x00201044) /* Interupt Clear Register */

void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(char *str);
void uart_send_hex(unsigned int num);
void uart_printf(char *fmt, ...);
void uart_send_num(int64_t num, int base, int type);
void uart_irq_on();
void uart_irq_off();
void uart_irq_send(char *str);
void uart_irq_read(char *str);
void write_handler();
void recv_handler();

#endif /*_P_MINI_UART_H */