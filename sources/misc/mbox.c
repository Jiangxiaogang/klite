/*
* 描述: 简易的消息邮箱
*       没有实现消息列队，只能保存一个状态值
* 应用场景: 常用于"命令-应答"模式的程序逻辑, 一个线程负责发命令并等待应答，另一个线程负责接收应答
* 作者: 蒋晓岗<kerndev@foxmail.com>
*/
#include "kernel.h"
#include "mbox.h"

struct mbox_object
{
    uint32_t data;
    event_t  event;
};

mbox_t mbox_create(void)
{
    struct mbox_object *obj;
    obj = heap_alloc(sizeof(struct mbox_object));
    if(obj != NULL)
    {
        obj->data = 0;
        obj->event= event_create(false, false);
        if(obj->event)
        {
            return obj;
        }
        heap_free(obj);
    }
    return NULL;
}

void mbox_delete(mbox_t mbox)
{
    struct mbox_object *obj;
    obj = (struct mbox_object *)mbox;
    event_delete(obj->event);
    heap_free(obj);
}

void mbox_post(mbox_t mbox, uint32_t data)
{
    struct mbox_object* obj;
    obj = (struct mbox_object*)mbox;
    obj->data = data;
    event_set(obj->event);
}

void mbox_wait(mbox_t mbox, uint32_t *pdata)
{
    struct mbox_object *obj;
    obj = (struct mbox_object *)mbox;
    event_wait(obj->event);
    *pdata = obj->data;
}

bool mbox_timedwait(mbox_t mbox, uint32_t timeout, uint32_t *pdata)
{
    bool ret;
    struct mbox_object *obj;
    obj = (struct mbox_object *)mbox;
    ret = event_timedwait(obj->event, timeout);
    *pdata = obj->data;
    return ret;
}
