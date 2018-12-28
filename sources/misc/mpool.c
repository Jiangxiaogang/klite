/*
* 描述: 块格式的内存池
*       提供高效的,内存大小固定的,动态内存申请与释放,
*		用一个FIFO管理内存节点,避免频繁查表,提高运行效率.
* 作者: 蒋晓岗<kerndev@foxmail.com>
*/
#include "kernel.h"
#include "mpool.h"

mpool_t mpool_create(uint32_t blk_size, uint32_t blk_count)
{
	uint32_t i;
	uint32_t addr;
	mpool_t  mpool;
	
	mpool = heap_alloc((blk_size + sizeof(void *)) * blk_count + sizeof(struct mpool));
	if(mpool == NULL)
	{
		return NULL;
	}
    
	mpool->mutex= mutex_create();
	if(mpool->mutex == NULL)
	{
		heap_free(mpool);
		return NULL;
	}
	
	mpool->blk_addr = (uint32_t *)(mpool + 1);
	addr = (uint32_t)(mpool->blk_addr + blk_count);
	mpool->next_free  = 0;
	mpool->next_alloc = 0;
	mpool->blk_free   = 0;
	mpool->blk_count  = blk_count;
	for(i = 0; i < blk_count; i++)
	{
		mpool_free(mpool, (void *)addr);
		addr += blk_size;
	}
	return mpool;
}

void mpool_delete(mpool_t mpool)
{
	mutex_delete(mpool->mutex);
	heap_free(mpool);
}

void *mpool_alloc(mpool_t mpool)
{
	uint32_t addr;

	if(mpool->blk_free > 0)
	{
		mutex_lock(mpool->mutex);
		addr = mpool->blk_addr[mpool->next_alloc];
		if(++mpool->next_alloc >= mpool->blk_count)
		{
			mpool->next_alloc = 0;
		}
		mpool->blk_free--;
		mutex_unlock(mpool->mutex);
		return (void *)addr;
	}
	return NULL;
}

void mpool_free(mpool_t mpool, void *mem)
{
	uint32_t addr;

	addr = (uint32_t)mem;
	mutex_lock(mpool->mutex);
	mpool->blk_addr[mpool->next_free] = addr;
	if(++mpool->next_free >= mpool->blk_count)
	{
		mpool->next_free = 0;
	}
	mpool->blk_free++;
	mutex_unlock(mpool->mutex);
}

