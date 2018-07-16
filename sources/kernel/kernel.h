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
#ifndef __KERNEL_H
#define __KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef void *thread_t;
typedef void *mutex_t;
typedef void *event_t;
typedef void *tasklet_t;

/******************************************************************************
* kernel
******************************************************************************/
void     kernel_init(void);
void     kernel_start(void);
void     kernel_timetick(uint32_t time);
uint32_t kernel_time(void);
uint32_t kernel_version(void);

/******************************************************************************
* heap
******************************************************************************/
void     heap_init(uint32_t heap_addr, uint32_t heap_size);
void    *heap_alloc(uint32_t size);
void     heap_free(void *p);
void     heap_usage(uint32_t *total, uint32_t *used);

/******************************************************************************
* thread
******************************************************************************/
#define THREAD_PRIORITY_MAX     (+127)
#define THREAD_PRIORITY_MIN     (-127)
#define THREAD_PRIORITY_DEFAULT (0)

#define THREAD_STACK_DEFAULT    (256)
thread_t thread_create(void (*entry)(void *), void *arg, uint32_t stack_size);
void     thread_delete(thread_t thread);
void     thread_suspend(thread_t thread);
void     thread_resume(thread_t thread);
void     thread_setprio(thread_t thread, int prio);
int      thread_getprio(thread_t thread);
uint32_t thread_time(thread_t thread);
void     thread_sleep(uint32_t time);
void     thread_yield(void);
void     thread_exit(void);
thread_t thread_self(void);

/******************************************************************************
* mutex
******************************************************************************/
mutex_t  mutex_create(void);
void     mutex_delete(mutex_t mutex);
void     mutex_lock(mutex_t mutex);
void     mutex_unlock(mutex_t mutex);

/******************************************************************************
* event
******************************************************************************/
event_t  event_create(bool state);
void     event_delete(event_t event);
void     event_post(event_t event);
void     event_wait(event_t event);
bool     event_timedwait(event_t event, uint32_t timeout);
void     event_wakeone(event_t event);
void     event_wakeall(event_t event);
void     event_keepalive(event_t event);
void     event_clear(event_t event);

/******************************************************************************
* tasklet
******************************************************************************/
void      tasklet_init(uint32_t stack_size);
tasklet_t tasklet_create(void (*func)(void *), void *data);
void      tasklet_delete(tasklet_t tasklet);
void      tasklet_schedule(tasklet_t tasklet);

/******************************************************************************
* alias
******************************************************************************/
#define malloc       heap_alloc
#define free         heap_free
#define sleep        thread_sleep

#endif
