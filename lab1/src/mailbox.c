#include "mailbox.h"
#include "mini_uart.h"

int mailbox_call(unsigned int *mailbox) {
    unsigned int address = (unsigned int)(((unsigned long)mailbox & ~0xF) | (MBOX_CH_PROP & 0xF));

    /* Check if Mailbox 0 status register’s full flag is set */
    /* If not, then can write to Mailbox 1 Read/Write register */
    while(*MAILBOX_STATUS & MAILBOX_FULL);

    *MAILBOX_WRITE = address;

    while(1) {
        /* Check if Mailbox 0 status register’s empty flag is set. */
        /* If not, then you can read from Mailbox 0 Read/Write register */
        while (*MAILBOX_STATUS & MAILBOX_EMPTY);

        /* Check if the value is the same as MAILBOX_READ */
        if(address == *MAILBOX_READ)
            return mailbox[1] == MBOX_RESPONSE;
    }
    
    return 0;
}

void get_board_revision() {
    unsigned int mailbox[8] __attribute__((aligned(16)));
    
    /* Get board revision */
    mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    
    mailbox[2] = GET_BOARD_REVISION;
    mailbox[3] = 4;                  // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0;                  // value buffer
    mailbox[6] = END_TAG;

    if(mailbox_call(mailbox)) {
        uart_send_string("Board Revision: 0x"); // it should be 0xa020d3 for rpi3 b+
        uart_send_hex(mailbox[5]);
        uart_send_string("\r\n");
    }
    else {
        uart_send_string("Failed to get Board Revision\r\n");
    }
}

void get_arm_memory() {
    unsigned int mailbox[8] __attribute__((aligned(16)));

    /* Get ARM memory */
    mailbox[0] = 8 * 4; // 8 * 4-byte
    mailbox[1] = REQUEST_CODE;
    mailbox[2] = GET_ARM_MEMORY;
    mailbox[3] = 8;                 // maximum of request and response value buffer's length (8 bytes)
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0;                 // ARM Memory Base Address
    mailbox[6] = 0;                 // ARM Memory Size
    mailbox[7] = END_TAG;

    if(mailbox_call(mailbox)) {
        uart_send_string("ARM memory base address: 0x");
        uart_send_hex(mailbox[5]);
        uart_send_string("\r\n");

        uart_send_string("ARM memory size: 0x");
        uart_send_hex(mailbox[6]);
        uart_send_string("\r\n");
    }
    else {
        uart_send_string("Failed to get ARM memory info\r\n");
    }
}