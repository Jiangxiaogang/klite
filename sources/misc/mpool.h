#ifndef __MPOOL_H
#define __MPOOL_H

typedef void *mpool_t;

mpool_t mpool_create(uint32_t block_size, uint32_t block_count);
void    mpool_delete(mpool_t pool);
void   *mpool_alloc(mpool_t pool);
void    mpool_free(mpool_t pool, void *mem);

#endif
