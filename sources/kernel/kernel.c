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

#define MAKE_VERSION_CODE(a,b,c)    ((a<<24)|(b<<16)|(c))
#define KERNEL_VERSION_CODE         MAKE_VERSION_CODE(4,0,0)

static uint32_t m_tick_count;
static thread_t m_idle_thread;

extern void heap_init(uint32_t addr, uint32_t size);
extern void thread_clean(void);

void kernel_init(uint32_t heap_addr, uint32_t heap_size)
{
    m_tick_count = 0;
    m_idle_thread = NULL;
    sched_init();
    heap_init(heap_addr, heap_size);
}

void kernel_start(void)
{
    sched_switch();
}

void kernel_idle(void)
{
    m_idle_thread = thread_self();
    thread_set_priority(m_idle_thread, THREAD_PRIORITY_IDLE - 1);
    while(1)
    {
        thread_clean();
        sched_idle();
    }
}

uint32_t kernel_idle_time(void)
{
    if(m_idle_thread != NULL)
    {
        return thread_time(m_idle_thread);
    }
    return 0;
}

uint32_t kernel_time(void)
{
    return m_tick_count;
}

void kernel_time_tick(uint32_t time)
{
    m_tick_count += time;
    sched_time_tick(time);
    sched_preempt();
}

uint32_t kernel_version(void)
{
    return KERNEL_VERSION_CODE;
}
