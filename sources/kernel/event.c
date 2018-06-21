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

struct event
{
    struct tcb_node *head;
    struct tcb_node *tail;
    bool state;
    bool manual;
};

event_t event_create(bool state, bool manual)
{
    struct event *obj;
    obj = heap_alloc(sizeof(struct event));
    if(obj != NULL)
    {
        obj->head   = NULL;
        obj->tail   = NULL;
        obj->state  = state;
        obj->manual = manual;
    }
    return (event_t)obj;
}

void event_delete(event_t event)
{
    heap_free(event);
}

void event_wait(event_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    if(obj->state)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
        sched_unlock();
        return;
    }
    object_wait((struct object *)obj, sched_tcb_now);
    sched_unlock();
    sched_switch();
}

bool event_timedwait(event_t event, uint32_t timeout)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    if(obj->state)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
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

void event_set(event_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    if(object_wake_all((struct object *)obj))
    {
        obj->state = obj->manual;
        sched_unlock();
        sched_preempt();
    }
    else
    {
        obj->state = true;
        sched_unlock();
    }
}

void event_reset(event_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    obj->state = false;
    sched_unlock();
}
