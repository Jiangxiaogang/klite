/******************************************************************************
* Copyright (c) 2015-2017 jiangxiaogang<kerndev@foxmail.com>
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

typedef void*   kthread_t;
typedef void*   kmutex_t;
typedef void*   kevent_t;
typedef void*   ksem_t;

/******************************************************************************
* kernel
******************************************************************************/
void        kernel_init(uint32_t mem_addr, uint32_t mem_size);
void        kernel_start(void);
uint32_t    kernel_time(void);
uint32_t    kernel_idletime(void);
uint32_t    kernel_version(void);

/******************************************************************************
* memory
******************************************************************************/
void*       kmem_alloc(uint32_t size);
void        kmem_free(void* mem);
void        kmem_info(uint32_t* total, uint32_t* used);

/******************************************************************************
* thread
******************************************************************************/
kthread_t   kthread_create(void(*func)(void*), void* arg, uint32_t stk_size);
void        kthread_destroy(kthread_t thread);
void        kthread_suspend(kthread_t thread);
void        kthread_resume(kthread_t thread);
void        kthread_setprio(kthread_t thread, int prio);
int         kthread_getprio(kthread_t thread);
uint32_t    kthread_time(kthread_t thread);
void        kthread_sleep(uint32_t tick);
void        kthread_exit(void);
kthread_t   kthread_self(void);

/******************************************************************************
* mutex
******************************************************************************/
kmutex_t    kmutex_create(void);
void        kmutex_destroy(kmutex_t mutex);
void        kmutex_lock(kmutex_t mutex);
void        kmutex_unlock(kmutex_t mutex);

/******************************************************************************
* event
******************************************************************************/
kevent_t    kevent_create(int state);
void        kevent_destroy(kevent_t event);
void        kevent_post(kevent_t event);
void        kevent_wait(kevent_t event);
int         kevent_timedwait(kevent_t event, uint32_t timeout);

/******************************************************************************
* semaphore
******************************************************************************/
ksem_t      ksem_create(uint32_t value);
void        ksem_destroy(ksem_t sem);
void        ksem_post(ksem_t sem);
void        ksem_wait(ksem_t sem);
int         ksem_timedwait(ksem_t sem, uint32_t timeout);
uint32_t    ksem_getvalue(ksem_t sem);

/******************************************************************************
* alias
******************************************************************************/
#define     clock       kernel_time
#define     sleep       kthread_sleep
#define     malloc      kmem_alloc
#define     free        kmem_free

#endif
