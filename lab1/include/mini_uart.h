#ifndef _P_MINI_UART_H
#define _P_MINI_UART_H

#include "base.h"

#define AUX_ENABLES (PBASE + 0x00215004)
#define AUX_MU_IO_REG (PBASE + 0x00215040) // used to write data to and read data from the UART FIFOs
#define AUX_MU_IER_REG (PBASE + 0x00215044) // 0:Disable interrupt because currently you don’t need interrupt.
#define AUX_MU_IIR_REG (PBASE + 0x00215048) // Set AUX_MU_IIR_REG to 6.
#define AUX_MU_LCR_REG (PBASE + 0x0021504C) // Set the data size.
#define AUX_MU_MCR_REG (PBASE + 0x00215050) // 0:Don’t need auto flow control.
#define AUX_MU_LSR_REG (PBASE + 0x00215054) // shows the data status
#define AUX_MU_CNTL_REG (PBASE + 0x00215060) // transmitter and receiver
#define AUX_MU_BAUD_REG (PBASE + 0x00215068) // 270:Set baud rate to 115200

void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(char *str);
void uart_send_hex(unsigned int num);

#endif /*_P_MINI_UART_H */