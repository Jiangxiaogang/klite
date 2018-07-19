/****************************************************************************
* 描述: 计数信号量
*       用于特殊场合的线程同步
* 蒋晓岗<kerndev@foxmail.com>
* 2018.07.16 创建文件
****************************************************************************/
#include "kernel.h"
#include "semaphore.h"

bool sem_init(sem_t *sem, uint32_t value)
{
    sem->event = event_create();
    sem->mutex = mutex_create();
    sem->value = value;
    return true;
}

void sem_delete(sem_t *sem)
{
    event_delete(sem->event);
    mutex_delete(sem->mutex);
}

void sem_wait(sem_t *sem)
{
    mutex_lock(sem->mutex);
    if(sem->value != 0)
    {
        sem->value--;
        mutex_unlock(sem->mutex);
    }
    else
    {
        mutex_unlock(sem->mutex);
        event_wait(sem->event);
    }
}

bool sem_timedwait(sem_t *sem, uint32_t timeout)
{
    mutex_lock(sem->mutex);
    if(sem->value != 0)
    {
        sem->value--;
        mutex_unlock(sem->mutex);
        return true;
    }
    else
    {
        mutex_unlock(sem->mutex);
        return event_timedwait(sem->event, timeout);
    }
}

void sem_post(sem_t *sem)
{
    mutex_lock(sem->mutex);
    if(!event_wakeone(sem->event))
    {
        sem->value++;
    }
    mutex_unlock(sem->mutex);
}
