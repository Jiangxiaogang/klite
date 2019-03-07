/******************************************************************************
* Copyright (c) 2015-2019 jiangxiaogang<kerndev@foxmail.com>
*
* This file is part of KLite distribution.
*
* KLite is free software, you can redistribute it and/or modify it under
* the MIT Licence.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************/
#include "kernel.h"
#include "stm32f0xx.h"

#define CPU_FREQ_MHZ 48

//系统初始化
void cpu_sys_init(void)
{
    RCC_HSEConfig(RCC_HSE_OFF);	
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12); 
	RCC_PLLCmd(ENABLE);  
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);     
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  
	while(RCC_GetSYSCLKSource() != 0x08);
    
    SysTick_Config(CPU_FREQ_MHZ * 1000);
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
	NVIC_SetPriority(PendSV_IRQn, 255);
	NVIC_SetPriority(SysTick_IRQn, 255);
}

//系统空闲
void cpu_sys_idle(uint32_t time)
{
    //__disable_irq();
    __wfi();
    //__enable_irq();
}

//系统滴答
void SysTick_Handler(void)
{
    kernel_timetick(1);
}
