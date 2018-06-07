#ifndef __FIFO_H
#define __FIFO_H

typedef void *fifo_t;

fifo_t   fifo_create(uint32_t size);
void     fifo_delete(fifo_t ff);
uint32_t fifo_read(fifo_t ff, void *buf, uint32_t len);
uint32_t fifo_write(fifo_t ff, void *buf, uint32_t len);
uint32_t fifo_overflow(fifo_t ff);
void     fifo_clear(fifo_t ff);

#endif
