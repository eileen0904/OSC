#include "_task.h"
#include "mini_uart.h"
#include "irq.h"

#include <stddef.h>

task *task_head;

void create_task(task_callback callback, int priority) {
    task *new_task = simple_alloc(sizeof(task));
    new_task->callback = callback;
    new_task->p = priority;
    uart_printf("created a task with priority %d\r\n", new_task->p);
    new_task->started = 0;

    push_task(new_task);
}

void push_task(task *t) {
    asm volatile("msr DAIFSet, 0xf"); // disable irq

    if(task_head == NULL) {
        task_head = simple_alloc(sizeof(task));
        task_head->next = NULL;
    }

    task *cur = task_head;
    while(cur->next != NULL && cur->next->p > t->p) {
        cur = cur->next;
    }

    t->next = cur->next;
    cur->next = t;

    asm volatile("msr DAIFClr, 0xf"); // enable irq
}

void execute_tasks() {
    while(task_head->next != NULL && !task_head->next->started) {
        irq(0);
        task_head->next->started = 1;
        irq(1);
        uart_printf("Task with priority %d started\r\n", task_head->next->p);
        task_head->next->callback();
        uart_printf("Task with priority %d ended\r\n", task_head->next->p);
        task_head = task_head->next;
    }
}