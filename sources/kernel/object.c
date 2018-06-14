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
#include "list.h"
#include "object.h"

void object_wait(struct object *obj, struct tcb *tcb)
{
    tcb->state = TCB_STATE_WAIT;
    tcb->lwait = (struct tcb_list *)obj;
    list_append(obj, tcb->nwait);
}

void object_wait_timeout(struct object *obj, struct tcb *tcb, uint32_t timeout)
{
    tcb->state = TCB_STATE_TIMEDWAIT;
    tcb->lwait = (struct tcb_list *)obj;
    list_append(obj, tcb->nwait);
    sched_tcb_sleep(tcb, timeout);
}

bool object_wake_one(struct object *obj)
{
    struct tcb *tcb;
    if(obj->head)
    {
        tcb = obj->head->tcb;
        tcb->lwait = NULL;
        list_remove(obj, tcb->nwait);
        sched_tcb_ready(tcb);
        return true;
    }
    return false;
}

bool object_wake_all(struct object *obj)
{
    struct tcb *tcb;
    if(obj->head)
    {
        while(obj->head)
        {
            tcb = obj->head->tcb;
            tcb->lwait = NULL;
            list_remove(obj, tcb->nwait);
            sched_tcb_ready(tcb);
        }
        return true;
    }
    return false;
}
