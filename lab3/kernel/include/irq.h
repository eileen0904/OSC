#ifndef _P_IRQ_H
#define _P_IRQ_H

#include "base.h"

#define IRQ_BASIC_PENDING (PBASE + 0x0000B200)
#define IRQ_PENDING_1 (PBASE + 0x0000B204)
#define IRQ_PENDING_2 (PBASE + 0x0000B208)
#define FIQ_CONTROL (PBASE + 0x0000B20C)
#define ENABLE_IRQS_1 (PBASE + 0x0000B210)
#define ENABLE_IRQS_2 (PBASE + 0x0000B214)
#define ENABLE_BASIC_IRQS (PBASE + 0x0000B218)
#define DISABLE_IRQS_1 (PBASE + 0x0000B21C)
#define DISABLE_IRQS_2 (PBASE + 0x0000B220)
#define DISABLE_BASIC_IRQS (PBASE + 0x0000B224)
#define CORE0_INT_SRC (0x40000060)
#define CORE0_TIMER_IRQ_CTRL (0x40000040)

void irq(int d);
void change_read_irq(int d);
void change_write_irq(int d);

#endif /*_P_IRQ_H */