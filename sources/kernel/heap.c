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

#define MEM_ALIGN_BYTE      (4)
#define MEM_ALIGN_MASK      (MEM_ALIGN_BYTE - 1)
#define MEM_ALIGN_PAD(m)    (((m) + MEM_ALIGN_MASK) & (~MEM_ALIGN_MASK))
#define MEM_ALIGN_CUT(m)    ((m) & (~MEM_ALIGN_MASK))

struct heap_mutex
{
    struct tcb_node *head;
    struct tcb_node *tail;
    bool lock;
};

struct heap_node
{
    struct heap_node *prev;
    struct heap_node *next;
    uint32_t used;
};

static uint32_t           m_heap_size;
static struct heap_node  *m_heap_head;
static struct heap_node  *m_heap_free;
static struct heap_mutex *m_heap_mutex;

static void heap_lock(void)
{
    sched_lock();
    if(m_heap_mutex->lock == false)
    {
        m_heap_mutex->lock = true;
        sched_unlock();
        return;
    }
    sched_tcb_wait(sched_tcb_now, (struct tcb_list *)m_heap_mutex);
    sched_unlock();
    sched_switch();
}

static void heap_unlock(void)
{
    sched_lock();
    if(sched_tcb_wake_one((struct tcb_list *)m_heap_mutex))
    {
        sched_unlock();
        sched_preempt();
    }
    else
    {
        m_heap_mutex->lock = false;
        sched_unlock();
    }
}

static struct heap_node * find_next_free(struct heap_node *node)
{
    uint32_t free;
    for(; node->next!=NULL; node=node->next)
    {
        free = ((uint32_t)node->next) - ((uint32_t)node) - node->used;
        if(free > sizeof(struct heap_node))
        {
            break;
        }
    }
    return node;
}

void heap_init(uint32_t addr, uint32_t size)
{
    uint32_t start;
    uint32_t end;
    
    start = MEM_ALIGN_PAD(addr);
    end   = MEM_ALIGN_CUT(addr + size);
    m_heap_size = end - start;
    m_heap_head = (struct heap_node *)start;
    m_heap_head->used = sizeof(struct heap_node) + sizeof(struct heap_mutex);
    m_heap_head->prev = NULL;
    m_heap_head->next = (struct heap_node *)(end - sizeof(struct heap_node));
    m_heap_head->next->used = sizeof(struct heap_node);
    m_heap_head->next->prev = m_heap_head;
    m_heap_head->next->next = NULL;
    m_heap_free = m_heap_head;
    
    m_heap_mutex = (struct heap_mutex *)(m_heap_head + 1);
    m_heap_mutex->head = NULL;
    m_heap_mutex->tail = NULL;
    m_heap_mutex->lock = false;
}

void *heap_alloc(uint32_t size)
{
    uint32_t free;
    uint32_t need;
    struct heap_node *temp;
    struct heap_node *node;
    
    need = size + sizeof(struct heap_node);
    need = MEM_ALIGN_PAD(need);
    heap_lock();
    for(node = m_heap_free; node->next != NULL; node = node->next)
    {
        free = ((uint32_t)node->next) - ((uint32_t)node) - node->used;
        if(free >= need)
        {
            temp = (struct heap_node *)((uint32_t)node + node->used);
            temp->prev = node;
            temp->next = node->next;
            temp->used = need;
            node->next->prev = temp;
            node->next = temp;
            if(node == m_heap_free)
            {
                m_heap_free = find_next_free(m_heap_free);
            }
            heap_unlock();
            return (void *)(temp + 1);
        }
    }
    heap_unlock();
    return NULL;
}

void heap_free(void *mem)
{
    struct heap_node *node;
    node = (struct heap_node *)mem - 1;
    heap_lock();
    node->prev->next = node->next;
    if(node->prev < m_heap_free)
    {
        m_heap_free = node->prev;
    }
    heap_unlock();
}

void heap_usage(uint32_t *total, uint32_t *used)
{
    struct heap_node *node;

    *used  = 0;
    *total = m_heap_size;
    heap_lock();
    for(node=m_heap_head; node!=NULL; node=node->next)
    {
        *used += node->used;
    }
    heap_unlock();
}
