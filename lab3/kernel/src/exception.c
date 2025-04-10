#include "exception.h"
#include "func.h"
#include "irq.h"
#include "mini_uart.h"
#include "utils.h"
#include "timer.h"
#include <stddef.h>

void el1_interrupt_enable() {
    __asm__ __volatile__("msr daifclr, 0xf"); // umask all DAIF
}

void el1_interrupt_disable() {
    __asm__ __volatile__("msr daifset, 0xf"); // mask all DAIF
}

void invalid_exception_router(unsigned long long x0) {}

void el1h_irq_router() {
    // uart exception
    if(get32(IRQ_PENDING_1) & (1 << 29) && get32(CORE0_INT_SRC) & (1 << 8)) {
        if(get32(AUX_MU_IIR_REG) & (1 << 1)) {
            put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(2)); // disable write interrupt
            irqtask_add(uart_w_irq_handler, 1); // 1: uart irq priority
            irqtask_run_preemptive(); // run the queued task before returning to the program.
        } 
        else if(get32(AUX_MU_IIR_REG) & (2 << 1)) {
            put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(1)); // disable read interrupt
            irqtask_add(uart_r_irq_handler, 1);
            irqtask_run_preemptive();
        }
    }
    // core timer exception
    else if(get32(CORE0_INT_SRC) & (1 << 1)) {
        core_timer_disable();
        irqtask_add(core_timer_handler, 0); // 0: timer irq priority
        irqtask_run_preemptive();
        core_timer_enable();
    }
}

void el0_sync_router() {
    unsigned long long spsr_el1;
    __asm__ __volatile__("mrs %0, SPSR_EL1\n\t" : "=r" (spsr_el1)); // to enable interrupt
    unsigned long long elr_el1;
    __asm__ __volatile__("mrs %0, ELR_EL1\n\t" : "=r" (elr_el1));   // holds the address if return to EL1
    unsigned long long esr_el1;
    __asm__ __volatile__("mrs %0, ESR_EL1\n\t" : "=r" (esr_el1));   // to know why the exception happens.
    uart_printf("[Exception][el0_sync] spsr_el1 : 0x%x, elr_el1 : 0x%x, esr_el1 : 0x%x\n", spsr_el1, elr_el1, esr_el1);
}

void el0_irq_64_router() {
    // uart exception
    if(get32(IRQ_PENDING_1) & (1 << 29) && get32(CORE0_INT_SRC) & (1 << 8)) {
        if(get32(AUX_MU_IIR_REG) & (0b01 << 1)) {
            put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(2)); // disable write interrupt
            irqtask_add(uart_w_irq_handler, 1);
            irqtask_run_preemptive(); // run the queued task before returning to the program.
        } 
        else if(get32(AUX_MU_IIR_REG) & (0b10 << 1)) {
            put32(AUX_MU_IER_REG, get32(AUX_MU_IER_REG) & ~(1)); // disable read interrupt
            irqtask_add(uart_r_irq_handler, 1);
            irqtask_run_preemptive();
        }
    }
    // core timer exception
    else if(get32(CORE0_INT_SRC) & (1 << 1)) {
        core_timer_disable();
        irqtask_add(core_timer_handler, 0);
        irqtask_run_preemptive();
        core_timer_enable();
    }
}

/*--------------Preemption--------------*/

int curr_task_priority = 9999; // Small number has higher priority
struct list_head *task_list;
void irqtask_list_init() { INIT_LIST_HEAD(task_list); }

void irqtask_add(void *task_function, unsigned long long priority) {
    irqtask_t *the_task = simple_alloc(sizeof(irqtask_t));

    the_task->priority = priority;
    the_task->task_function = task_function;
    INIT_LIST_HEAD(&the_task->listhead);

    // add the timer_event into timer_event_list (sorted)
    // if the priorities are the same -> FIFO
    struct list_head *curr;

    el1_interrupt_disable();
    // enqueue the processing task to the event queue with sorting.
    list_for_each(curr, task_list) {
        if(((irqtask_t *)curr)->priority > the_task->priority) {
            list_add(&the_task->listhead, curr->prev);
            break;
        }
    }
    // if the priority is lowest
    if(list_is_head(curr, task_list)) {
        list_add_tail(&the_task->listhead, task_list);
    }

    el1_interrupt_enable();
}

void irqtask_run_preemptive() {
    el1_interrupt_enable();

    while(!list_empty(task_list)) {
        el1_interrupt_disable(); // critical section protects new coming node

        irqtask_t *the_task = (irqtask_t *)task_list->next;
        // Run new task (early return) if its priority is lower than the scheduled task.
        if(curr_task_priority <= the_task->priority) {
            el1_interrupt_enable();
            break;
        }
        // get the scheduled task and run it.
        list_del_entry((struct list_head *)the_task);
        int prev_task_priority = curr_task_priority;
        curr_task_priority = the_task->priority;

        el1_interrupt_enable();
        irqtask_run(the_task);
        el1_interrupt_disable();

        curr_task_priority = prev_task_priority;
        el1_interrupt_enable();
    }
}

void irqtask_run(irqtask_t* the_task) {
    // 將 void* 轉型成 void (*func_ptr)() 型別並呼叫
    ((void (*)())the_task->task_function)();
}