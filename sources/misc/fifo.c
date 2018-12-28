/*
* 无锁先进先出环形缓冲区
* 蒋晓岗<kerndev@foxmail.com>
*/
#include "kernel.h"
#include "fifo.h"

fifo_t fifo_create(uint32_t size)
{
	fifo_t fifo;
	fifo = heap_alloc(sizeof(struct fifo) + size);
	if(fifo != NULL)
	{
		fifo->buff = (uint8_t *)(fifo + 1);
		fifo->size = size;
		fifo->tx   = 0;
		fifo->rx   = 0;
		return fifo;
	}
	return NULL;
}

void fifo_delete(fifo_t fifo)
{
	heap_free(fifo);
}

uint32_t fifo_write(fifo_t fifo, void *buf, uint32_t len)
{
	uint32_t i;
	uint32_t rx;
	uint8_t  *p = buf;
    
    for(i = 0; i < len; i++)
	{
		rx = fifo->rx + 1;
		if(rx == fifo->size)
		{
			rx = 0;
		}
		if(rx == fifo->tx)
		{
			break;
		}
		fifo->buff[fifo->rx] = *p++;
		fifo->rx = rx;
	}
	return i;
}

uint32_t fifo_read(fifo_t fifo, void *buf, uint32_t len)
{
	uint32_t i;
	uint8_t  *p = buf;
	
	for(i = 0; i < len; i++)
	{
		if(fifo->tx == fifo->rx)
		{
			break;
		}
		*p++ = fifo->buff[fifo->tx++];
		if(fifo->tx == fifo->size)
		{
			fifo->tx = 0;
		}
	}
	return i;
}

void fifo_clear(fifo_t fifo)
{
	fifo->tx = 0;
	fifo->rx = 0;
}
