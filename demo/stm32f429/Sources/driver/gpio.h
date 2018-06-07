#ifndef __GPIO_H
#define __GPIO_H

enum GPIO_PORT
{
	PA=0,
	PB,
	PC,
	PD,
	PE,
	PF,
	PG,
	PH,
	PI,
	PJ,
	PK,
};

#define GPIO_MODE_IN		0
#define GPIO_MODE_OUT		1
#define GPIO_MODE_AF		2
#define GPIO_MODE_AN		3

#define GPIO_IN_FLOAT		0
#define GPIO_IN_PULLUP		1
#define	GPIO_IN_PULLDOWN	2

#define GPIO_OUT_PP			0
#define GPIO_OUT_OD			1

void gpio_init(int port);
void gpio_open(int port, int pin, int mode, int type);
void gpio_close(int port, int pin);
void gpio_write(int port, int pin, int value);
int  gpio_read(int port, int pin);

#endif
