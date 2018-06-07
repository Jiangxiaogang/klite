#ifndef __NVIC_H
#define __NVIC_H

void nvic_init(void);
void nvic_open(int irq, int prio);
void nvic_close(int irq);

#endif
