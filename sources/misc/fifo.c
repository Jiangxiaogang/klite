/*
* 描述: 先进先出队列
*       可动态创建任意大小的队列.
* 作者: 蒋晓岗<kerndev@foxmail.com>
*/
#include "kernel.h"
#include "fifo.h"

struct fifo
{
    uint8_t *buff;
    uint32_t size;
    uint32_t in;
    uint32_t out;
    uint32_t overflow;
};

//创建FIFO
fifo_t fifo_create(uint32_t size)
{
    struct fifo *fifo;
    fifo = heap_alloc(sizeof(struct fifo) + size);
    if(fifo != NULL)
    {
        fifo->buff = (uint8_t *)(fifo+1);
        fifo->size = size;
        fifo->in   = 0;
        fifo->out  = 0;
        fifo->overflow = 0;
        return fifo;
    }
    return NULL;
}

//删除FIFO
void fifo_delete(fifo_t ff)
{
    heap_free(ff);
}

//清空FIFO
void fifo_clear(fifo_t ff)
{
    struct fifo *fifo;
    fifo = (struct fifo *)ff;
    fifo->in   = 0;
    fifo->out  = 0;
    fifo->overflow = 0;
}

//返回溢出次数
uint32_t fifo_overflow(fifo_t ff)
{
    struct fifo *fifo;
    fifo = (struct fifo *)ff;
    return fifo->overflow;
}

//从FIFO读数据
uint32_t fifo_read(fifo_t ff, void *buf, uint32_t len)
{
    uint32_t i;
    uint32_t end;
    uint8_t *data;
    struct fifo *fifo;
    
    fifo = (struct fifo *)ff;
    data = buf;
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

//向FIFO写数据
uint32_t fifo_write(fifo_t ff, void *buf, uint32_t len)
{
    uint32_t i;
    uint32_t next;
    uint8_t *data;
    struct fifo * fifo;

    fifo = (struct fifo *)ff;
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
