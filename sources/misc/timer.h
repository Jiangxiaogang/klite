#ifndef __TIMER_H
#define __TIMER_H

typedef void *timer_t;

void    timer_init(uint32_t stk_size, int prio);
timer_t timer_create(void);
void    timer_delete(timer_t timer);
void    timer_start(timer_t timer, uint32_t timeout, void (*handler)(void *), void *arg);
void    timer_stop(timer_t timer);

#endif
