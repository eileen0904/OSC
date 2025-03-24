#ifndef _MAILBOX_H
#define _MAILBOX_H

#include "base.h"

/* Mailbox_0 : CPU read from GPU */
/* Mailbox_1 : CPU write to GPU */

#define MAILBOX_BASE (PBASE + 0xb880)
#define MAILBOX_READ ((volatile unsigned int *)(MAILBOX_BASE)) // Mailbox_0
#define MAILBOX_STATUS ((volatile unsigned int *)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE ((volatile unsigned int *)(MAILBOX_BASE + 0x20)) // Mailbox_1

#define MAILBOX_EMPTY 0x40000000
#define MAILBOX_FULL 0x80000000
#define MBOX_RESPONSE 0x80000000
#define MBOX_CH_PROP 8

#define MBOX_REQUEST 0

/* Tags (ARM to VC) */
#define MBOX_TAG_LAST 0
#define GET_BOARD_REVISION 0x00010002
#define GET_ARM_MEMORY 0x00010005
#define REQUEST_CODE 0x00000000
#define REQUEST_SUCCEED 0x80000000
#define REQUEST_FAILED 0x80000001
#define TAG_REQUEST_CODE 0x00000000
#define END_TAG 0x00000000

int mailbox_call(unsigned int *mailbox);
void get_board_revision();
void get_arm_memory();

#endif /* _MAILBOX_H */