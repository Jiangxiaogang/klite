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

static void list_init(void* list)
{
	((struct __list*)list)->head = NULL;
	((struct __list*)list)->tail = NULL;
}

static void list_append(void* list, void* node)
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

static void list_remove(void* list, void* node)
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

static void list_insert(void* list, void* after, void* node)
{
	if(after == NULL)
	{
		((struct __node*)node)->prev = after;
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

static void list_move(void* list, void* after, void* node)
{
	if(after != node)
	{
		list_remove(list,node);
		list_insert(list,after,node);
	}
}

#endif
