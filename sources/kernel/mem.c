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

#define MEM_ALIGN_BYTE       (4)
#define MEM_ALIGN_MASK      (MEM_ALIGN_BYTE-1)
#define MEM_ALIGN_PAD(m)    (((m)+MEM_ALIGN_MASK) & (~MEM_ALIGN_MASK))
#define MEM_ALIGN_CUT(m)    ((m) & (~MEM_ALIGN_MASK))

struct mem_lock
{
    struct tcb_node *head;
    struct tcb_node *tail;
    int data;
};

struct mem_node
{
    uint32_t used;
    struct mem_node *next;
};

static uint32_t         m_mem_size;
static struct mem_node *m_mem_head;
static struct mem_lock  m_mem_lock;

void kmem_init(uint32_t addr, uint32_t size)
{
    uint32_t start;
    uint32_t end;
    
    start    = MEM_ALIGN_PAD(addr);
    end      = MEM_ALIGN_CUT(start+size);
    m_mem_size= end - start;
    
    m_mem_head = (struct mem_node *)start;
    m_mem_head->used = sizeof(struct mem_node);
    m_mem_head->next = (struct mem_node *)(end - sizeof(struct mem_node));
    m_mem_head->next->used = sizeof(struct mem_node);
    m_mem_head->next->next = NULL;
    
    m_mem_lock.head = NULL;
    m_mem_lock.tail = NULL;
    m_mem_lock.data = 0;
}

static void kmem_lock(void)
{
    sched_lock();
    if(m_mem_lock.data == 0)
    {
        m_mem_lock.data = 1;
        sched_unlock();
        return;
    }
    sched_tcb_wait(sched_tcb_now, &m_mem_lock);
    sched_unlock();
    sched_switch();
}

static void kmem_unlock(void)
{
    sched_lock();
    if(m_mem_lock.head == NULL)
    {
        m_mem_lock.data = 0;
        sched_unlock();
        return;
    }
    sched_tcb_ready(m_mem_lock.head->tcb);
    sched_unlock();
}

void *kmem_alloc(uint32_t size)
{
    void *mem;
    uint32_t free;
    uint32_t need;
    struct mem_node *tmp;
    struct mem_node *node;
    
    mem  = NULL;
    size = MEM_ALIGN_PAD(size);
    need = size + sizeof(struct mem_node);

    kmem_lock();
    for(node=m_mem_head; node->next!=NULL; node=node->next)
    {
        free = ((uint32_t)node->next) - ((uint32_t)node) - node->used;
        if(free >= need)
        {
            tmp = (struct mem_node *)((uint32_t)node + node->used);
            tmp->next = node->next;
            tmp->used = need;
            node->next = tmp;
            mem = (void *)((uint32_t)(tmp+1));
            break;
        }
    }
    kmem_unlock();
    return mem;
}

void kmem_free(void *mem)
{
    struct mem_node *node;
    struct mem_node *prev;
    struct mem_node *find;
    
    prev = m_mem_head;
    find = ((struct mem_node *)mem) - 1;

    kmem_lock();
    for(node=m_mem_head->next; node->next!=NULL; node=node->next)
    {
        if(node == find)
        {
            prev->next = node->next;
            break;
        }
        prev = node;
    }
    kmem_unlock();
}

void kmem_usage(uint32_t *total, uint32_t *used)
{
    struct mem_node *node;
    
    *used  = 0;
    *total = m_mem_size;
    
    kmem_lock();
    for(node=m_mem_head; node!=NULL; node=node->next)
    {
        *used += node->used;
    }
    kmem_unlock();
}
