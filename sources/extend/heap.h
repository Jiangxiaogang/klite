#ifndef __HEAP_H
#define __HEAP_H

typedef void* heap_t;

heap_t heap_create(uint32_t size);
void   heap_destroy(heap_t heap);
void*  heap_alloc(heap_t heap, uint32_t size);
void   heap_free(heap_t heap, void* mem);
void   heap_info(heap_t heap, uint32_t* total, uint32_t* used);

#endif
