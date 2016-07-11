/******************************************************************************
* lowlevel cpu arch functions
* Copyright (C) 2015-2016 jiangxiaogang <kerndev@foxmail.com>
*
* This file is part of klite.
* 
* klite is free software; you can redistribute it and/or modify it under the 
* terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* klite is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with klite; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
******************************************************************************/
#include "stm32f4xx.h"

#define CPU_FREQ_MHZ 168

void cpu_init(void)
{
	RCC_HSICmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_HSIRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	
	RCC_PLLCmd(DISABLE);
	RCC_HSEConfig(RCC_HSE_OFF);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY));
	RCC_HSEConfig(RCC_HSE_ON);
	while(!RCC_GetFlagStatus(RCC_FLAG_HSERDY));

	RCC_PLLConfig(RCC_PLLSource_HSE, 16, CPU_FREQ_MHZ*2, 2, 7);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	
	FLASH_PrefetchBufferCmd(ENABLE);
	FLASH_SetLatency(FLASH_Latency_5);
	
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div4);
	RCC_PCLK2Config(RCC_HCLK_Div2);

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_HSICmd(DISABLE);
	
	#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
	#endif
}

void cpu_core_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_SetPriority(PendSV_IRQn, 255);
}

void cpu_tick_init(void)
{
	SysTick_Config(CPU_FREQ_MHZ*1000);
	NVIC_SetPriority(SysTick_IRQn, 255);
}
