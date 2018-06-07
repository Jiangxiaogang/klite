/*
* 描述: 应用层内存管理
*       
* 应用场景: 由于过多的申请和释放内存会降低内核的内存管理效率
*           所以在应用层可以为每个功能模块单独创建一个专用的内存管理器.
*
* 作者: 蒋晓岗<kerndev@foxmail.com>
*/
#include "kernel.h"
#include "heap.h"

#define MEMORY_ALIGN    4
#define MEMORY_ALIGN_MASK   (MEMORY_ALIGN-1)
#define MEMORY_NODE_MAGIC   0x1234ABCD

struct heap_node
{
    uint32_t magic;
    uint32_t used;
    struct heap_node *next;
};

struct heap_obj
{
    uint32_t size;
    kmutex_t mutex;
    struct heap_node *head;
};

static void heap_init(struct heap_obj *obj, void *addr, uint32_t size)
{
    uint32_t start;
    uint32_t end;
    
    start = (uint32_t)addr;
    end = start + size;
    start = (start+MEMORY_ALIGN_MASK) & (~MEMORY_ALIGN_MASK);
    end = end & (~MEMORY_ALIGN_MASK);
    obj->size = end - start;
    
    obj->head = (struct heap_node *)start;
    obj->head->magic= MEMORY_NODE_MAGIC;
    obj->head->used = sizeof(struct heap_node);
    obj->head->next = (struct heap_node *)(end - sizeof(struct heap_node));
    obj->head->next->magic = MEMORY_NODE_MAGIC;
    obj->head->next->used = sizeof(struct heap_node);
    obj->head->next->next = NULL;
}

heap_t heap_create(uint32_t size)
{
    void* buff;
    struct heap_obj *obj;
    obj = kmem_alloc(sizeof(struct heap_obj) + size);
    if(obj!=NULL)
    {
        buff = obj+1;
        obj->mutex = kmutex_create();
        heap_init(obj, buff, size);
    }
    return obj;
}

void heap_delete(heap_t heap)
{
    struct heap_obj *obj;
    obj = heap;
    kmutex_delete(obj->mutex);
    kmem_free(obj);
}

void *heap_alloc(heap_t heap, uint32_t size)
{
    void *ret;
    uint32_t free;
    uint32_t need;
    struct heap_node *node;
    struct heap_node *tmp;
    struct heap_obj  *obj;
    
    obj = heap;
    ret = NULL;
    need = size + sizeof(struct heap_node);
    need = (need+MEMORY_ALIGN_MASK) & (~(MEMORY_ALIGN_MASK));
    kmutex_lock(obj->mutex);
    for(node=obj->head; node->next!=NULL; node=node->next)
    {
        free = (uint32_t)node->next - (uint32_t)node - node->used;
        if(free >= need)
        {
            tmp = (struct heap_node *)((uint32_t)node + node->used);
            tmp->next  = node->next;
            tmp->used  = need;
            tmp->magic = MEMORY_NODE_MAGIC;
            node->next = tmp;
            ret = (void *)((uint32_t)(tmp + 1));
            break;
        }
    }
    kmutex_unlock(obj->mutex);
    return ret;
}

void heap_free(heap_t heap, void *mem)
{
    struct heap_obj  *obj;
    struct heap_node *node;
    struct heap_node *prev;
    struct heap_node *find;
    
    obj  = heap;
    prev = obj->head;
    node = (struct heap_node *)mem-1;
    if(node->magic != MEMORY_NODE_MAGIC)
    {
        return;
    }
    kmutex_lock(obj->mutex);
    for(find=prev->next; find->next!=NULL; find=find->next)
    {
        if(find == node)
        {
            node->magic= ~MEMORY_NODE_MAGIC;
            prev->next = find->next;
            break;
        }
        prev = find;
    }
    kmutex_unlock(obj->mutex);
}

void heap_usage(heap_t heap, uint32_t *total, uint32_t *used)
{
    struct heap_obj *obj;
    struct heap_node *node;
    obj = heap;
    *used  = 0;
    *total = obj->size;
    kmutex_lock(obj->mutex);
    for(node=obj->head; node!=NULL; node=node->next)
    {
        *used += node->used;
    }
    kmutex_unlock(obj->mutex);
}
