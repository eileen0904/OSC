#include "timer.h"
#include "func.h"
#include "irq.h"
#include "mini_uart.h"

#define STR(x) #x      // 把「參數字面值」變成字串，不會展開
#define XSTR(x) STR(x) // 強迫展開後再轉字串

struct list_head *timer_event_list; // first head has nothing, store timer_event_t after it

void timer_list_init() { INIT_LIST_HEAD(timer_event_list); }

void core_timer_enable() {
    __asm__ __volatile__(
        "mov x1, 1\n\t"
        "msr cntp_ctl_el0, x1\n\t"
        "mov x2, 2\n\t"
        "ldr x1, =" XSTR(CORE0_TIMER_IRQ_CTRL) "\n\t"
        "str w2, [x1]\n\t"         
    );
}

void core_timer_disable() {
    __asm__ __volatile__(
        "mov x2, 0\n\t"
        "ldr x1, =" XSTR(CORE0_TIMER_IRQ_CTRL) "\n\t"
        "str w2, [x1]\n\t"         
    );
}

void core_timer_handler() {
    if(list_empty(timer_event_list)) {
        set_core_timer_interrupt(10000); // // disable timer interrupt 沒有事件就設一個很大的值
        return;
    }
    timer_event_callback((timer_event_t *)timer_event_list->next); // do callback and set new interrupt
}

void timer_event_callback(timer_event_t *timer_event) {
    list_del_entry((struct list_head*)timer_event); // delete the event in queue
    ((void (*)(char*))timer_event-> callback)(timer_event->args);  // call the event

    // set queue linked list to next time event if it exists
    if(!list_empty(timer_event_list)) {
        set_core_timer_interrupt_by_tick(((timer_event_t*)timer_event_list->next)->interrupt_time);
    }
    else {
        set_core_timer_interrupt(10000);  // disable timer interrupt
    }
}

// set timer interrupt time to [expired_time] seconds after now (relatively)
void set_core_timer_interrupt(unsigned long long expired_time) {
    __asm__ __volatile__(
        "mrs x1, cntfrq_el0\n\t" // frequency of the timer
        "mul x1, x1, %0\n\t"     // relative timer to cntfrq_el0
        "msr cntp_tval_el0, x1\n\t" // Set expired time to cntp_tval_el0, which stores time value of EL1 physical timer.
        : "=r"(expired_time)
    );
}

// directly set timer interrupt time to a cpu tick  (directly)
void set_core_timer_interrupt_by_tick(unsigned long long tick) {
    __asm__ __volatile__(
        "msr cntp_cval_el0, %0\n\t" // cntp_cval_el0 -> absolute timer
        : "=r"(tick)
    );
}

unsigned long long get_tick_plus_s(unsigned long long second) {
    unsigned long long cntpct_el0=0;
    // 讀取目前 tick 值
    __asm__ __volatile__("mrs %0, cntpct_el0\n\t": "=r"(cntpct_el0)); 
    unsigned long long cntfrq_el0=0;
    // 讀取 tick 頻率（每秒幾個 tick）
    __asm__ __volatile__("mrs %0, cntfrq_el0\n\t": "=r"(cntfrq_el0)); // tick frequency
    // 算出「現在 + 幾秒」後的 tick 值，作為觸發 timer 中斷的時間。
    return (cntpct_el0 + cntfrq_el0 * second);
}


void timer_set2sAlert(char* str) {
    unsigned long long cntpct_el0;
    __asm__ __volatile__("mrs %0, cntpct_el0\n\t": "=r"(cntpct_el0));
    
    unsigned long long cntfrq_el0;
    __asm__ __volatile__("mrs %0, cntfrq_el0\n\t": "=r"(cntfrq_el0)); // tick frequency

    uart_printf("[Interrupt][el1_irq][%s] %d seconds after booting\n", str, cntpct_el0 / cntfrq_el0);
    add_timer(timer_set2sAlert, 2, "2sAlert");
}

void add_timer(void *callback, unsigned long long timeout, char *args) {
    timer_event_t* the_timer_event = simple_alloc(sizeof(timer_event_t)); // free by timer_event_callback
    
    // store all the related information in timer_event
    the_timer_event->args = simple_alloc(strlen(args) + 1);
    strncpy(the_timer_event -> args, args, 0, strlen(args));
    the_timer_event->interrupt_time = get_tick_plus_s(timeout);
    the_timer_event->callback = callback;
    INIT_LIST_HEAD(&the_timer_event->listhead);

    // add the timer_event into timer_event_list (sorted)
    struct list_head* curr;
    list_for_each(curr,timer_event_list) {
        if(((timer_event_t*)curr)->interrupt_time > the_timer_event->interrupt_time) {
            list_add(&the_timer_event->listhead,curr->prev);  // add this timer at the place just before the bigger one (sorted)
            break;
        }
    }
    // if the timer_event is the biggest, run this code block
    if(list_is_head(curr,timer_event_list)) {
        list_add_tail(&the_timer_event->listhead,timer_event_list);
    }
    // 設定下一個 timer 中斷（永遠是第一個事件）
    set_core_timer_interrupt_by_tick(((timer_event_t*)timer_event_list->next)->interrupt_time);
}
