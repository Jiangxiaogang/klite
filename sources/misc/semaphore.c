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
#include "scheduler.h"

struct semaphore
{
    struct tcb_node *head;
    struct tcb_node *tail;
    uint32_t value;
    uint32_t limit;
};

sem_t sem_create(uint32_t init_value, uint32_t max_value)
{
    struct semaphore *p_sem;
    p_sem = heap_alloc(sizeof(struct semaphore));
    if(p_sem != NULL)
    {
        p_sem->head  = NULL;
        p_sem->tail  = NULL;
        p_sem->value = init_value;
        p_sem->limit = max_value;
    }
    return (sem_t)p_sem;
}

void sem_delete(sem_t sem)
{
    heap_free(sem);
}

void sem_wait(sem_t sem)
{
    struct semaphore *p_sem;
    p_sem = (struct semaphore *)sem;
    sched_lock();
    if(p_sem->value != 0)
    {
        p_sem->value--;
        sched_unlock();
        return;
    }
    sched_tcb_wait(sched_tcb_now, (struct tcb_list *)p_sem);
    sched_unlock();
    sched_switch();
}

bool sem_timedwait(sem_t sem, uint32_t timeout)
{
    struct semaphore *p_sem;
    p_sem = (struct semaphore *)sem;
    sched_lock();
    if(p_sem->value != 0)
    {
        p_sem->value--;
        sched_unlock();
        return true;
    }
    if(timeout == 0)
    {
        sched_unlock();
        return false;
    }
    sched_tcb_timedwait(sched_tcb_now, (struct tcb_list *)p_sem, timeout);
    sched_unlock();
    sched_switch();
    return (sched_tcb_now->timeout != 0);
}

void sem_post(sem_t sem)
{
    struct semaphore *p_sem;
    p_sem = (struct semaphore *)sem;
    sched_lock();
    if(sched_tcb_wake_one((struct tcb_list *)p_sem))
    {
        sched_unlock();
        sched_preempt();
    }
    else
    {
        if(p_sem->value < p_sem->limit)
        {
            p_sem->value++;
        }
        sched_unlock();
    }
}

void sem_clear(sem_t sem)
{
    struct semaphore *p_sem;
    p_sem = (struct semaphore *)sem;
    sched_lock();
    p_sem->value = 0;
    sched_unlock();
}

uint32_t sem_getvalue(sem_t sem)
{
    struct semaphore *p_sem;
    p_sem = (struct semaphore *)sem;
    return p_sem->value;
}
