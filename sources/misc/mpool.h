#ifndef __MPOOL_H
#define __MPOOL_H

struct mpool
{
    mutex_t   mutex;
	uint32_t* blk_addr;
    uint32_t  blk_count;
    uint32_t  blk_free;
	uint32_t  next_alloc;
    uint32_t  next_free;
};

typedef struct mpool * mpool_t;

mpool_t mpool_create(uint32_t blk_size, uint32_t blk_count);
void    mpool_delete(mpool_t pool);
void*   mpool_alloc(mpool_t pool);
void    mpool_free(mpool_t pool, void *mem);

#endif

