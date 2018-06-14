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
#include "object.h"

struct sem
{
    struct tcb_node *head;
    struct tcb_node *tail;
    uint32_t value;
    uint32_t limit;
};

sem_t sem_create(uint32_t init_value, uint32_t max_value)
{
    struct sem *obj;
    obj = heap_alloc(sizeof(struct sem));
    if(obj != NULL)
    {
        obj->head  = NULL;
        obj->tail  = NULL;
        obj->value = init_value;
        obj->limit = max_value;
    }
    return (sem_t)obj;
}

void sem_delete(sem_t sem)
{
    heap_free(sem);
}

void sem_wait(sem_t sem)
{
    struct sem *obj;
    obj = (struct sem *)sem;
    sched_lock();
    if(obj->value != 0)
    {
        obj->value--;
        sched_unlock();
        return;
    }
    object_wait((struct object *)obj, sched_tcb_now);
    sched_unlock();
    sched_switch();
}

bool sem_timedwait(sem_t sem, uint32_t timeout)
{
    struct sem *obj;
    obj = (struct sem *)sem;
    sched_lock();
    if(obj->value != 0)
    {
        obj->value--;
        sched_unlock();
        return true;
    }
    if(timeout == 0)
    {
        sched_unlock();
        return false;
    }
    object_wait_timeout((struct object *)obj, sched_tcb_now, timeout);
    sched_unlock();
    sched_switch();
    return (sched_tcb_now->timeout != 0);
}

void sem_post(sem_t sem)
{
    struct sem *obj;
    obj = (struct sem *)sem;
    sched_lock();
    if(!object_wake_one((struct object *)obj))
    {
        if(obj->value < obj->limit)
        {
            obj->value++;
        }
    }
    sched_unlock();
}

uint32_t sem_getvalue(sem_t sem)
{
    struct sem *obj;
    obj = (struct sem *)sem;
    return obj->value;
}
