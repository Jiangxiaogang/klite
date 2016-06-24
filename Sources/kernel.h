/******************************************************************************
* header file of klite kernel.
* Copyright (C) 2015-2016 jiangxiaogang <kerndev@foxmail.com>
* 
* This file is part of klite.
*
* klite is free software; you can redistribute it and/or modify it under the 
* terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* klite is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with klite; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
******************************************************************************/
#ifndef __KERNEL_H
#define __KERNEL_H

#include <stddef.h>
#include <stdint.h>

typedef void*	kthread_t;
typedef void*	kmutex_t;
typedef void*	kevent_t;
typedef void*	ksem_t;

/******************************************************************************
* kernel
******************************************************************************/
void 		kernel_init(uint32_t mem_addr, uint32_t mem_size);
void 		kernel_start(void);
uint32_t    kernel_time(void);
uint32_t	kernel_version(void);

/******************************************************************************
* memory
******************************************************************************/
void*  		kmem_alloc(uint32_t size);
void   		kmem_free(void* mem);
void   		kmem_info(uint32_t* total, uint32_t* used);

/******************************************************************************
* thread
******************************************************************************/
kthread_t 	kthread_create(void(*func)(void*),void* arg,uint32_t stk_size);
void   		kthread_destroy(kthread_t thread);
void 		kthread_setprio(kthread_t thread, int prio);
int	        kthread_getprio(kthread_t thread);
void   		kthread_exit(void);
kthread_t 	kthread_self(void);
void		kthread_sleep(uint32_t ms);

/******************************************************************************
* mutex
******************************************************************************/
kmutex_t  	kmutex_create(void);
void   		kmutex_destroy(kmutex_t mutex);
void   		kmutex_lock(kmutex_t mutex);
void   		kmutex_unlock(kmutex_t mutex);

/******************************************************************************
* event
******************************************************************************/
kevent_t 	kevent_create(int state);
void   		kevent_destroy(kevent_t event);
void   		kevent_post(kevent_t event);
void   		kevent_wait(kevent_t event);
int     	kevent_timedwait(kevent_t event, uint32_t timeout);

/******************************************************************************
* semaphore
******************************************************************************/
ksem_t 		ksem_create(int value);
void   		ksem_destroy(ksem_t sem);
void 		ksem_post(ksem_t sem);
void   		ksem_wait(ksem_t sem);
int   		ksem_timedwait(ksem_t sem, uint32_t timeout);
int			ksem_getvalue(ksem_t sem);

/******************************************************************************
* alias
******************************************************************************/
#define 	malloc	kmem_alloc
#define		free	kmem_free
#define		sleep	kthread_sleep
#define		clock	kernel_time

#endif
