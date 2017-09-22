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
#include "kernel.h"
#include "internal.h"
#include "port.h"

#define KERNEL_VERSION_CODE         MAKE_VERSION_CODE(2,2,6)

static uint32_t  tick_count;
static kthread_t idle_thread;

static void kernel_idle(void* arg)
{
    for(;;)
    {
        cpu_os_idle();
    }
}

void kernel_init(uint32_t mem_addr, uint32_t mem_size)
{
    cpu_os_init();
    ksched_init();
    kmem_init(mem_addr, mem_size);
    idle_thread = kthread_create(kernel_idle, 0, 0);
    kthread_setprio(idle_thread, THREAD_PRIORITY_MIN);
}

void kernel_start(void)
{
    tick_count = 0;
    cpu_os_start();
    ksched_execute();
    cpu_os_idle();
}

uint32_t kernel_time(void)
{
    return tick_count;
}

uint32_t kernel_idletime(void)
{
    return kthread_time(idle_thread);
}

uint32_t kernel_version(void)
{
    return KERNEL_VERSION_CODE;
}

void kernel_timetick(void)
{   
    tick_count++;
    ksched_timetick();
}

