#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include "u_list.h"

typedef struct irqtask {
    struct list_head listhead;
    unsigned long long priority; // store priority (smaller number is more preemptive)
    void *task_function; // task function pointer
} irqtask_t;

void el1_interrupt_enable();
void el1_interrupt_disable();
void invalid_exception_router(unsigned long long x0);
void el1h_irq_router();
void el0_sync_router();
void el0_irq_64_router();

void irqtask_list_init();
void irqtask_add(void *task_function, unsigned long long priority);
void irqtask_run_preemptive();
void irqtask_run(irqtask_t *the_task);

#endif /* _EXCEPTION_H */