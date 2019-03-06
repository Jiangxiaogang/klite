/*
* STM32F429ÄÚÖÃÎÂ¶È
* ADC1_IN18
* ½¯Ïþ¸Ú<kerndev@foxmail.com>
* 2019.3.5
*/
#include "stm32f4xx.h"
#include "gpio.h"
#include "temp.h"

static void adc_init(void)
{
	ADC_InitTypeDef init;
	ADC_CommonInitTypeDef common;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	common.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	common.ADC_Mode = ADC_Mode_Independent;
	common.ADC_Prescaler = ADC_Prescaler_Div8;
	common.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_8Cycles;
	ADC_CommonInit(&common);
	
    init.ADC_Resolution = ADC_Resolution_12b;
	init.ADC_ContinuousConvMode = DISABLE;
	init.ADC_DataAlign = ADC_DataAlign_Right;
	init.ADC_ExternalTrigConv = DISABLE;
	init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	init.ADC_NbrOfConversion = 1;
	init.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &init);
	ADC_Cmd(ADC1,ENABLE);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_480Cycles);
}

static int adc_read(void)
{
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}


void temp_init(void)
{
	adc_init();
}

int  temp_read(void)
{
	int adc;
	int vol;
	adc = adc_read();
	vol = adc * 3300 / 4096;
	return vol / 2.5 - 290;
}
