/*
* STM32F4xx GPIOÇý¶¯
* ½¯Ïþ¸Ú<kerndev@foxmail.com>
*/
#include "stm32f4xx.h"
#include "gpio.h"

#define GET_GPIO_CTX(x)	((GPIO_TypeDef*)(AHB1PERIPH_BASE + ((x)<<10)))

void gpio_open(int port, int pin, int mode, int type)
{
	GPIO_InitTypeDef io;
	
    io.GPIO_Mode  = (GPIOMode_TypeDef)mode;
	io.GPIO_PuPd  = (mode==GPIO_MODE_IN)?(GPIOPuPd_TypeDef)type:GPIO_PuPd_UP;
	io.GPIO_OType = (mode==GPIO_MODE_OUT)?(GPIOOType_TypeDef)type:GPIO_OType_PP;
	io.GPIO_Speed = GPIO_Speed_100MHz;
	io.GPIO_Pin   = 1<<pin;
	GPIO_Init(GET_GPIO_CTX(port),&io);
	if(mode==GPIO_MODE_AF)
	{
		GPIO_PinAFConfig(GET_GPIO_CTX(port), pin, type);
	}
}

void gpio_close(int port, int pin)
{
	GPIO_InitTypeDef io;
	
    io.GPIO_Mode  = GPIO_Mode_IN;
	io.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	io.GPIO_OType = GPIO_OType_OD;
	io.GPIO_Speed = GPIO_Speed_100MHz;
	io.GPIO_Pin   = 1<<pin;
	GPIO_Init(GET_GPIO_CTX(port),&io);
}

int gpio_read(int port, int pin)
{
	return (((GET_GPIO_CTX(port)->IDR)>>pin)&0x01);
}

void gpio_write(int port, int pin, int value)
{
	if(value)
		GET_GPIO_CTX(port)->BSRRL = (1<<pin);
	else
		GET_GPIO_CTX(port)->BSRRH = (1<<pin);
}

void gpio_init(int port)
{
	RCC_AHB1PeriphClockCmd(1<<port, ENABLE);
}
