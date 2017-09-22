/*
* 描述: 先进先出队列
*       可动态创建任意大小的队列.
* 作者: 蒋晓岗<kerndev@foxmail.com>
*/
#include "kernel.h"
#include "fifo.h"

struct fifo
{
    char* buff;
    int   size;
    int   in;
    int   out;
    int   overflow;
};

fifo_t fifo_create(int size)
{
    struct fifo* fifo;
    fifo = kmem_alloc(sizeof(struct fifo)+size);
    if(fifo != NULL)
    {
        fifo->buff = (char*)(fifo+1);
        fifo->size = size;
        fifo->in   = 0;
        fifo->out  = 0;
        fifo->overflow = 0;
        return fifo;
    }
    return NULL;
}

void fifo_destroy(fifo_t ff)
{
    kmem_free(ff);
}

void fifo_clear(fifo_t ff)
{
    struct fifo* fifo;
    fifo = (struct fifo*)ff;
    fifo->in   = 0;
    fifo->out  = 0;
    fifo->overflow = 0;
}

int fifo_overflow(fifo_t ff)
{
    struct fifo* fifo;
    fifo = (struct fifo*)ff;
    return fifo->overflow;
}

int fifo_read(fifo_t ff, void* buf, int len)
{
    int i;
    int end;
    char* data;
    struct fifo* fifo;
    
    data = buf;
    fifo = ff;
    end  = fifo->in;
    for(i=0; i<len; i++)
    {
        if(fifo->out == end)
        {
            break;
        }
        *data++ = fifo->buff[fifo->out++];
        if(fifo->out == fifo->size)
        {
            fifo->out = 0;
        }
    }
    return i;
}

int fifo_write(fifo_t ff, void* buf, int len)
{
    int i;
    int next;
    char* data;
    struct fifo* fifo;

    fifo = ff;
    data = buf;
    for(i=0; i<len; i++)
    {
        next = fifo->in+1;
        if(next == fifo->size)
        {
            next = 0;
        }
        if(next == fifo->out)
        {
            fifo->overflow++;
            break;
        }
        fifo->buff[fifo->in] = *data++;
        fifo->in = next;
    }
    return i;
}
