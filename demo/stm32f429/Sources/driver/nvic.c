#include "stm32f4xx.h"
#include "nvic.h"

void nvic_open(int irq, int prio)
{
	NVIC_InitTypeDef  nvic;
    nvic.NVIC_IRQChannel = irq;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = prio;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void nvic_close(int irq)
{
	NVIC_InitTypeDef  nvic;
    nvic.NVIC_IRQChannel = irq;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&nvic);
}

void nvic_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
}
