#ifndef __FIFO_H
#define __FIFO_H

struct fifo
{
	uint8_t * buff;
    uint32_t  size;
    uint32_t  tx;
    uint32_t  rx;
};

typedef struct fifo *fifo_t;

fifo_t   fifo_create(uint32_t size);
void     fifo_delete(fifo_t fifo);
uint32_t fifo_write(fifo_t fifo, void *buf, uint32_t len);
uint32_t fifo_read(fifo_t fifo, void *buf, uint32_t len);
void     fifo_clear(fifo_t fifo);

#endif
