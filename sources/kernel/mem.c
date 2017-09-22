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

#define MEM_ALIGN_MASK      (MEMORY_ALIGN_BYTE-1)
#define MEM_ALIGN_PAD(m)    (((m)+MEM_ALIGN_MASK) & (~MEM_ALIGN_MASK))
#define MEM_ALIGN_CUT(m)    ((m) & (~MEM_ALIGN_MASK))

struct mnode
{
    uint32_t used;
    struct mnode* next;
};

static uint32_t      kmem_size;
static struct mnode* kmem_head;
static struct object kmem_mutex;

void kmem_init(uint32_t addr, uint32_t size)
{
    uint32_t start;
    uint32_t end;
    
    start    = MEM_ALIGN_PAD(addr);
    end      = MEM_ALIGN_CUT(start+size);
    kmem_size= end - start;
    
    kmem_head = (struct mnode*)start;
    kmem_head->used = sizeof(struct mnode);
    kmem_head->next = (struct mnode*)(end - sizeof(struct mnode));
    kmem_head->next->used = sizeof(struct mnode);
    kmem_head->next->next = NULL;
    
    kobject_init(&kmem_mutex);
}

static void kmem_lock(void)
{
    ksched_lock();
    if(kmem_mutex.data == 0)
    {
        kmem_mutex.data = 1;
        ksched_unlock();
        return;
    }
    kobject_wait(&kmem_mutex, sched_tcb_now);
    ksched_unlock();
    ksched_execute();
}

static void kmem_unlock(void)
{
    ksched_lock();
    if(kmem_mutex.head == NULL)
    {
        kmem_mutex.data = 0;
        ksched_unlock();
        return;
    }
    kobject_post(&kmem_mutex, kmem_mutex.head->tcb);
    ksched_unlock();
}

void* kmem_alloc(uint32_t size)
{
    void* mem;
    uint32_t free;
    uint32_t need;
    struct mnode* tmp;
    struct mnode* node;
    
    mem  = NULL;
    size = MEM_ALIGN_PAD(size);
    need = size + sizeof(struct mnode);

    kmem_lock();
    for(node=kmem_head; node->next!=NULL; node=node->next)
    {
        free = ((uint32_t)node->next) - ((uint32_t)node) - node->used;
        if(free >= need)
        {
            tmp = (struct mnode*)((uint32_t)node + node->used);
            tmp->next = node->next;
            tmp->used = need;
            node->next = tmp;
            mem = (void*)((uint32_t)(tmp+1));
            break;
        }
    }
    kmem_unlock();
    return mem;
}

void kmem_free(void* mem)
{
    struct mnode* node;
    struct mnode* prev;
    struct mnode* find;
    
    prev = kmem_head;
    find = ((struct mnode*)mem)-1;

    kmem_lock();
    for(node=kmem_head->next; node->next!=NULL; node=node->next)
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

void kmem_info(uint32_t* total, uint32_t* used)
{
    struct mnode* node;
    
    *used  = 0;
    *total = kmem_size;
    
    kmem_lock();
    for(node=kmem_head; node!=NULL; node=node->next)
    {
        *used += node->used;
    }
    kmem_unlock();
}
