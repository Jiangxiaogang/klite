#ifndef __FIFO_H
#define __FIFO_H

typedef void* fifo_t;

fifo_t fifo_create(int size);
void   fifo_destroy(fifo_t ff);
int    fifo_write(fifo_t ff, void* buf, int len);
int    fifo_read(fifo_t ff, void* buf, int len);
void   fifo_clear(fifo_t ff);

#endif
