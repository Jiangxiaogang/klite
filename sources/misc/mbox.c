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

bool mbox_init(mbox_t *mbox)
{
    mbox->data = 0;
    mbox->event= event_create(false);
    if(mbox->event != NULL)
    {
        return true;
    }
    return false;
}

void mbox_delete(mbox_t *mbox)
{
    event_delete(mbox->event);
}

void mbox_post(mbox_t *mbox, uint32_t data)
{
    mbox->data = data;
    event_post(mbox->event);
}

void mbox_wait(mbox_t *mbox, uint32_t *pdata)
{
    event_wait(mbox->event);
    *pdata = mbox->data;
}

bool mbox_timedwait(mbox_t *mbox, uint32_t *pdata, uint32_t timeout)
{
    bool ret;
    ret = event_timedwait(mbox->event, timeout);
    *pdata = mbox->data;
    return ret;
}
