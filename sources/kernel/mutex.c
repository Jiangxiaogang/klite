/******************************************************************************
* Copyright (c) 2015-2019 jiangxiaogang<kerndev@foxmail.com>
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

mutex_t mutex_create(void)
{
    struct mutex *p_mutex;
    p_mutex = heap_alloc(sizeof(struct mutex));
    if(p_mutex != NULL)
    {
        p_mutex->head = NULL;
        p_mutex->tail = NULL;
        p_mutex->lock = false;
    }
    return (mutex_t)p_mutex;
}

void mutex_delete(mutex_t mutex)
{
    heap_free(mutex);
}

void mutex_lock(mutex_t mutex)
{
    struct mutex *p_mutex;
    p_mutex = (struct mutex *)mutex;
    sched_lock();
    if(p_mutex->lock == false)
    {
        p_mutex->lock = true;
        sched_unlock();
        return;
    }
    sched_tcb_wait(sched_tcb_now, (struct tcb_list *)p_mutex);
    sched_unlock();
    sched_switch();
}

void mutex_unlock(mutex_t mutex)
{
    struct mutex *p_mutex;
    p_mutex = (struct mutex *)mutex;
    sched_lock();
    if(sched_tcb_wake_one((struct tcb_list *)p_mutex))
    {
        sched_unlock();
        sched_preempt();
    }
    else
    {
        p_mutex->lock = false;
        sched_unlock();
    }
}

bool mutex_try_lock(mutex_t mutex)
{
    struct mutex *p_mutex;
    p_mutex = (struct mutex *)mutex;
    sched_lock();
    if(p_mutex->lock == false)
    {
        p_mutex->lock = true;
        sched_unlock();
        return true;
    }
    sched_unlock();
    return false;
}
