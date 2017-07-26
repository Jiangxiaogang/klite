#ifndef __TIMER_H
#define __TIMER_H

typedef void* timer_t;

void    timer_init(uint32_t stk_size, int prio);
timer_t timer_create(int id, int elapse, void(*callback)(int));
void    timer_delete(timer_t timer);

#endif
