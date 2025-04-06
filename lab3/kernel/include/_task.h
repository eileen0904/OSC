#ifndef _TASK_H
#define _TASK_H

typedef void (*task_callback)(void);

typedef struct task {
    struct task *next;
    task_callback callback;
    int p, started;
} task;

void create_task(task_callback callback, int priority);
void execute_tasks();
void push_task(task *t);

#endif /* _TASK_H */