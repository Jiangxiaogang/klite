#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "uart.h"
#include "log.h"

#define UART_PORT		1
#define UART_BAUDRATE	115200
#define BUFF_LEN		1024

static mutex_t  output_mutex;
static char*    output_buffer;

void log_printf(const char *fmt, ...)
{
	int len;
	va_list va;
	mutex_lock(output_mutex);
	va_start(va, fmt);
	len = vsnprintf(output_buffer, BUFF_LEN, fmt, va);
	va_end(va);
	uart_write(UART_PORT, output_buffer, len);
	mutex_unlock(output_mutex);
}

void log_init(void)
{
	output_buffer = heap_alloc(BUFF_LEN);
	output_mutex  = mutex_create();
	uart_init(UART_PORT, NULL, 0);
	uart_open(UART_PORT, UART_BAUDRATE, 0);
}
