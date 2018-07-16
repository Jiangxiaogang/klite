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
#include "semaphore.h"

bool sem_init(sem_t *sem, uint32_t value)
{
    sem->event = event_create(0);
    sem->mutex = mutex_create();
    sem->value = value;
    return true;
}

void sem_delete(sem_t *sem)
{
    event_delete(sem->event);
    mutex_delete(sem->mutex);
}

void sem_wait(sem_t *sem)
{
    mutex_lock(sem->mutex);
    if(sem->value != 0)
    {
        sem->value--;
        mutex_unlock(sem->mutex);
    }
    else
    {
        mutex_unlock(sem->mutex);
        event_wait(sem->event);
    }
}

bool sem_timedwait(sem_t *sem, uint32_t timeout)
{
    mutex_lock(sem->mutex);
    if(sem->value != 0)
    {
        sem->value--;
        mutex_unlock(sem->mutex);
        return true;
    }
    else
    {
        mutex_unlock(sem->mutex);
        return event_timedwait(sem->event, timeout);
    }
}

void sem_post(sem_t *sem)
{
    mutex_lock(sem->mutex);
    if(!event_wakeone(sem->event))
    {
        sem->value++;
    }
    mutex_unlock(sem->mutex);
}
