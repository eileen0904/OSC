#ifndef TIMER_H
#define TIMER_H

typedef void (*timer_callback)(char *);

typedef struct timer {
    struct timer *next;
    char data[100];
    timer_callback callback;
    unsigned long long exp;
} timer;

void print_data(char *str);
void push_timer(unsigned long long exp, char *str);
void set_timeout(unsigned long long s, char *str);

#endif /* TIMER_H */