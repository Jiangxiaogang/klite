#ifndef __TIMER_H
#define __TIMER_H

typedef struct timer
{
    struct timer *prev;
    struct timer *next;
    uint32_t counter;
    uint32_t timeout;
    void   (*handler)(void *);
    void    *arg;
}timer_t;

void    timer_init(uint32_t stk_size, int prio);
void    timer_start(timer_t *timer, uint32_t timeout, void (*handler)(void *), void *arg);
void    timer_stop(timer_t *timer);

#endif
