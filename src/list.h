/******************************************************************************
* kernel list helper.
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
#ifndef __LIST_H
#define __LIST_H

struct __node
{
	struct __node* 	prev;
	struct __node* 	next;
};

struct __list
{
	struct __node* 	head;
	struct __node* 	tail;
};

static __inline void list_init(void* list)
{
	((struct __list*)list)->head = NULL;
	((struct __list*)list)->tail = NULL;
}

static __inline void list_append(void* list, void* node)
{
	if(((struct __list*)list)->head == NULL)
	{
		((struct __node*)node)->prev = NULL;
		((struct __node*)node)->next = NULL;
		((struct __list*)list)->head = node;
		((struct __list*)list)->tail = node;
	}
	else
	{
		((struct __node*)node)->prev = ((struct __list*)list)->tail;
		((struct __node*)node)->next = NULL;
		((struct __list*)list)->tail->next = node;
		((struct __list*)list)->tail =node;
	}
}

static __inline void list_remove(void* list, void* node)
{
	if(((struct __node*)node)->prev != NULL)
	{
		((struct __node*)node)->prev->next = ((struct __node*)node)->next;
	}
	else
	{
		((struct __list*)list)->head = ((struct __node*)node)->next;
	}

	if(((struct __node*)node)->next != NULL)
	{
		((struct __node*)node)->next->prev = ((struct __node*)node)->prev;
	}
	else
	{
		((struct __list*)list)->tail = ((struct __node*)node)->prev;
	}
}


static __inline void list_insert(void* list, void* after, void* node)
{
	if(after == NULL)
	{
		((struct __node*)node)->prev = NULL;
		((struct __node*)node)->next = ((struct __list*)list)->head;
		((struct __list*)list)->head = node;
	}
	else
	{
		((struct __node*)node)->prev = after;
		((struct __node*)node)->next = ((struct __node*)after)->next;
		((struct __node*)after)->next = node;
	}
	if(((struct __node*)node)->next == NULL)
	{
		((struct __list*)list)->tail = node;
	}
	else
	{
		((struct __node*)node)->next->prev = node;
	}
}

static __inline void list_move(void* list, void* after, void* node)
{
	if(after != node)
	{
		list_remove(list,node);
		list_insert(list,after,node);
	}
}

#endif

