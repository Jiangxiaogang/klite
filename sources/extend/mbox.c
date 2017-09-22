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
    int data;
    kevent_t event;
};

mbox_t mbox_create(void)
{
    struct mbox_object* obj;
    obj = kmem_alloc(sizeof(struct mbox_object));
    if(obj != NULL)
    {
        obj->data = 0;
        obj->event= kevent_create(0);
        if(obj->event)
        {
            return obj;
        }
        kmem_free(obj);
    }
    return NULL;
}

void mbox_delete(mbox_t mbox)
{
    struct mbox_object* obj;
    obj = (struct mbox_object*)mbox;
    kevent_destroy(obj->event);
    kmem_free(obj);
}

void mbox_post(mbox_t mbox, int data)
{
    struct mbox_object* obj;
    obj = (struct mbox_object*)mbox;
    obj->data = data;
    kevent_post(obj->event);
}

void mbox_wait(mbox_t mbox, int* pdata)
{
    struct mbox_object* obj;
    obj = (struct mbox_object*)mbox;
    kevent_wait(obj->event);
    *pdata = obj->data;
}

int mbox_timedwait(mbox_t mbox, int* pdata, int timeout)
{
    int ret;
    struct mbox_object* obj;
    obj = (struct mbox_object*)mbox;
    ret = kevent_timedwait(obj->event, timeout);
    *pdata = obj->data;
    return ret;
}
