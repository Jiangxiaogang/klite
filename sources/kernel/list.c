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
#include <stddef.h>
#include "list.h"

void list_init(void* list)
{
    ((struct list*)list)->head = NULL;
    ((struct list*)list)->tail = NULL;
}

void list_append(void* list, void* node)
{
    ((struct node*)node)->next = NULL;
    ((struct node*)node)->prev = ((struct list*)list)->tail;
    if(((struct list*)list)->head == NULL)
    {
        ((struct list*)list)->head = node;
        ((struct list*)list)->tail = node;
    }
    else
    {
        ((struct list*)list)->tail->next = node;
        ((struct list*)list)->tail = node;
    }
}

void list_insert(void* list, void* node)
{
    ((struct node*)node)->prev = NULL;
    ((struct node*)node)->next = ((struct list*)list)->head;
    if(((struct list*)list)->head == NULL)
    {
        ((struct list*)list)->head = node;
        ((struct list*)list)->tail = node;
    }
    else
    {
        ((struct list*)list)->head->prev = node;
        ((struct list*)list)->head = node;
    }
}

void list_remove(void* list, void* node)
{
    if(((struct node*)node)->prev == NULL)
    {
        ((struct list*)list)->head = ((struct node*)node)->next;
    }
    else
    {
        ((struct node*)node)->prev->next = ((struct node*)node)->next;
    }
    if(((struct node*)node)->next == NULL)
    {
        ((struct list*)list)->tail = ((struct node*)node)->prev;
    }
    else
    {
        ((struct node*)node)->next->prev = ((struct node*)node)->prev;
    }
}

void list_insert_before(void* list, void* before, void* node)
{
    ((struct node*)node)->next = before;
    if(before == NULL)
    {
        ((struct node*)node)->prev = ((struct list*)list)->tail;
        ((struct list*)list)->tail = node;
    }
    else
    {
        ((struct node*)node)->prev = ((struct node*)before)->prev;
        ((struct node*)before)->prev = node;
    }
    if(((struct node*)node)->prev == NULL)
    {
        ((struct list*)list)->head = node;
    }
    else
    {
        ((struct node*)node)->prev->next = node;
    }
}

void list_insert_after(void* list, void* after, void* node)
{
    ((struct node*)node)->prev = after;
    if(after == NULL)
    {
        ((struct node*)node)->next = ((struct list*)list)->head;
        ((struct list*)list)->head = node;
    }
    else
    {
        ((struct node*)node)->next = ((struct node*)after)->next;
        ((struct node*)after)->next = node;
    }
    if(((struct node*)node)->next == NULL)
    {
        ((struct list*)list)->tail = node;
    }
    else
    {
        ((struct node*)node)->next->prev = node;
    }
}

