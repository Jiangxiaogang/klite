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

/******************************************************************************
* kernel
******************************************************************************/
void     kernel_init(uint32_t heap_addr, uint32_t heap_size);
void     kernel_start(void);
void     kernel_timetick(uint32_t time);
uint32_t kernel_time(void);
uint32_t kernel_version(void);

/******************************************************************************
* heap
******************************************************************************/
void*    heap_alloc(uint32_t size);
void     heap_free(void *p);
void     heap_usage(uint32_t *total, uint32_t *used);

/******************************************************************************
* thread
******************************************************************************/
#define THREAD_PRIORITY_REALTIME  (+3)
#define THREAD_PRIORITY_HIGHEST   (+2)
#define THREAD_PRIORITY_HIGH      (+1)
#define THREAD_PRIORITY_NORMAL    (0)
#define THREAD_PRIORITY_LOW       (-1)
#define THREAD_PRIORITY_LOWEST    (-2)
#define THREAD_PRIORITY_IDLE      (-3)

#define THREAD_STACK_DEFAULT      (1024)

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
void     thread_cleanup(void);

/******************************************************************************
* mutex
******************************************************************************/
mutex_t  mutex_create(void);
void     mutex_delete(mutex_t mutex);
void     mutex_lock(mutex_t mutex);
void     mutex_unlock(mutex_t mutex);
bool     mutex_trylock(mutex_t mutex);

/******************************************************************************
* event
******************************************************************************/
event_t  event_create(void);
void     event_delete(event_t event);
void     event_post(event_t event);
void     event_wait(event_t event);
bool     event_timedwait(event_t event, uint32_t timeout);
bool     event_wakeone(event_t event);
bool     event_wakeall(event_t event);
void     event_keepawake(event_t event);
void     event_reset(event_t event);

/******************************************************************************
* alias
******************************************************************************/
#define malloc       heap_alloc
#define free         heap_free
#define sleep        thread_sleep

#endif
