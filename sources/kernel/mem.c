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

#define MEMORY_ALIGN_BYTE	4
#define MEM_ALIGN_PAD(m)	(((m)+MEMORY_ALIGN_BYTE-1) & (~(MEMORY_ALIGN_BYTE-1)))
#define MEM_ALIGN_CUT(m)	((m) & (~(MEMORY_ALIGN_BYTE-1)))

struct mcb
{
	uint32_t used;
	struct mcb* next;
};

static uint32_t    kmem_size;
static struct mcb* kmem_mcb;

void kmem_init(uint32_t addr, uint32_t size)
{
	uint32_t start;
	uint32_t end;
	
	start    = MEM_ALIGN_PAD(addr);
	end      = MEM_ALIGN_CUT(start+size);
	kmem_size= end - start;
	
	kmem_mcb = (struct mcb*)start;
	kmem_mcb->used = sizeof(struct mcb);
	kmem_mcb->next = (struct mcb*)(end - sizeof(struct mcb));
	kmem_mcb->next->used = sizeof(struct mcb);
	kmem_mcb->next->next = NULL;
}

void* kmem_alloc(uint32_t size)
{
	struct mcb* mcb;
	struct mcb* tmp;
	void*    mem;
	uint32_t free;
	uint32_t need;
	
	mem  = NULL;
	size = MEM_ALIGN_PAD(size);
	need = size + sizeof(struct mcb);
	ksched_lock();
	for(mcb=kmem_mcb; mcb->next!=NULL; mcb=mcb->next)
	{
		free = ((uint32_t)mcb->next) - ((uint32_t)mcb) - mcb->used;
		if(free >= need)
		{
			tmp = (struct mcb*)((uint32_t)mcb + mcb->used);
			tmp->next = mcb->next;
			tmp->used = need;
			mcb->next = tmp;
			mem = (void*)((uint32_t)(tmp+1));
			break;
		}
	}
	ksched_unlock();
	return mem;
}

void kmem_free(void* mem)
{
	struct mcb* mcb;
	struct mcb* prev;
	struct mcb* find;
	
	prev = kmem_mcb;
	mcb  = ((struct mcb*)mem)-1;

	ksched_lock();
	for(find=kmem_mcb->next; find->next!=NULL; find=find->next)
	{
		if(find == mcb)
		{
			prev->next = find->next;
			break;
		}
		prev = find;
	}
	ksched_unlock();
}

void kmem_info(uint32_t* total, uint32_t* used)
{
	struct mcb* mcb;
	
	*used  = 0;
	*total = kmem_size;
	
	ksched_lock();
	for(mcb=kmem_mcb; mcb!=NULL; mcb=mcb->next)
	{
		*used += mcb->used;
	}
	ksched_unlock();
}
