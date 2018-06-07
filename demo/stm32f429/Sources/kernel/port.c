#include "stm32f4xx.h"

#define HSE_FREQ_MHZ 16
#define CPU_FREQ_MHZ 180

void cpu_os_init(void)
{
	RCC_HSICmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_HSIRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	
	RCC_PLLCmd(DISABLE);
	RCC_HSEConfig(RCC_HSE_OFF);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY));
	RCC_HSEConfig(RCC_HSE_ON);
	while(!RCC_GetFlagStatus(RCC_FLAG_HSERDY));

	RCC_PLLConfig(RCC_PLLSource_HSE, HSE_FREQ_MHZ, CPU_FREQ_MHZ*2, 2, 7);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	
	FLASH_PrefetchBufferCmd(ENABLE);
	FLASH_SetLatency(FLASH_Latency_5);
	
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div4);	//不超过45MHz
	RCC_PCLK2Config(RCC_HCLK_Div2);	//不超过90MHz
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_HSICmd(DISABLE);
}

void cpu_os_start(void)
{
	SysTick_Config(CPU_FREQ_MHZ*1000);
	NVIC_SetPriority(PendSV_IRQn, 255);
	NVIC_SetPriority(SysTick_IRQn, 255);
}

void cpu_os_idle(void)
{
	//__WFI();
}

