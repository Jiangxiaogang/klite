/******************************************************************************
* Copyright (c) 2015-2018 jiangxiaogang<kerndev@foxmail.com>
*
* This file is part of KLite distribution.
*
* KLite is free software, you can redistribute it and/or modify it under
* the MIT Licence.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************/
#include "kernel.h"
#include "sched.h"

struct mutex
{
    struct tcb_node *head;
    struct tcb_node *tail;
    bool lock;
};

kmutex_t kmutex_create(void)
{
    struct mutex *obj;
    obj = kmem_alloc(sizeof(struct mutex));
    if(obj != NULL)
    {
        obj->head = NULL;
        obj->tail = NULL;
        obj->lock = false;
    }
    return (kmutex_t)obj;
}

void kmutex_delete(kmutex_t mutex)
{
    kmem_free(mutex);
}

void kmutex_lock(kmutex_t mutex)
{
    struct mutex *obj;
    obj = (struct mutex *)mutex;
    sched_lock();
    if(obj->lock == false)
    {
        obj->lock = true;
        sched_unlock();
        return;
    }
    sched_tcb_wait(sched_tcb_now, obj);
    sched_unlock();
    sched_switch();
}

void kmutex_unlock(kmutex_t mutex)
{
    struct mutex *obj;
    obj = (struct mutex *)mutex;
    sched_lock();
    if(obj->head == NULL)
    {
        obj->lock = false;
        sched_unlock();
        return;
    }
    sched_tcb_ready(obj->head->tcb);
    sched_unlock();
}
