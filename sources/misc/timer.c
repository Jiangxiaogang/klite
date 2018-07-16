/****************************************************************************
* 描述: 软件定时器
*       可简化定时任务的代码编写
* 蒋晓岗<kerndev@foxmail.com>
* 2017.07.20 创建文件
* 2018.06.07 重写文件
****************************************************************************/
#include <string.h>
#include "kernel.h"
#include "list.h"
#include "timer.h"

struct timer
{
    struct timer *prev;
    struct timer *next;
    uint32_t counter;
    uint32_t timeout;
    void    *arg;
    void   (*handler)(void *);
};

struct timer_list
{
    struct timer *head;
    struct timer *tail;
    mutex_t mutex;
    event_t event;
};

static struct timer_list m_timer_list;

//返回下一个定时器超时时间
static uint32_t timer_timetick(uint32_t tick)
{
    uint32_t next_time;
    struct timer *node;
    next_time = 0xFFFFFFFF;
    mutex_lock(m_timer_list.mutex);
    for(node = m_timer_list.head; node!=NULL; node=node->next)
    {
        if(node->counter > tick)
        {
            node->counter -= tick;
        }
        else
        {
            node->handler(node->arg);
            node->counter = node->timeout;
        }
        if(node->counter < next_time)
        {
            next_time = node->counter;
        }
    }
    mutex_unlock(m_timer_list.mutex);
    return next_time;
}

//定时器线程
//最多等待下一个定时器发作这么久的时间，就要去检查一下!
static void timer_thread_entry(void *arg)
{
    uint32_t last;
    uint32_t tick;
    uint32_t timeout;
    last = kernel_time();
    while(1)
    {
        tick = kernel_time() - last;
        last = kernel_time();
        timeout = timer_timetick(tick);
        tick = kernel_time() - last;
        if(timeout > tick)
        {
            event_timedwait(m_timer_list.event, timeout - tick);
        }
    }
}

//创建定时器
timer_t timer_create(void)
{
    struct timer *node;
    node = heap_alloc(sizeof(struct timer));
    if(node != NULL)
    {
        memset(node, 0, sizeof(struct timer));
    }
    return node;
}

//删除定时器
void timer_delete(timer_t timer)
{
    timer_stop(timer);
    heap_free(timer);
}

//启动定时器
void timer_start(timer_t timer, uint32_t timeout, void (*handler)(void *), void *arg)
{
    struct timer *node;
    node = (struct timer *)timer;
    node->counter = timeout;
    node->timeout = timeout;
    node->arg     = arg;
    node->handler = handler;
    mutex_lock(m_timer_list.mutex);
    list_append(&m_timer_list, node);
    mutex_unlock(m_timer_list.mutex);
    event_post(m_timer_list.event);
}

//停止定时器
void timer_stop(timer_t timer)
{
    struct timer *node;
    node = (struct timer *)timer;
    mutex_lock(m_timer_list.mutex);
    list_remove(&m_timer_list, node);
    mutex_unlock(m_timer_list.mutex);
    event_post(m_timer_list.event);
}

//初始化定时器模块
//设置定时器线程的堆栈大小和线程优先级
void timer_init(uint32_t stk_size, int prio)
{
    thread_t thread;
    list_init(&m_timer_list);
    m_timer_list.mutex = mutex_create();
    m_timer_list.event = event_create(false);
    thread = thread_create(timer_thread_entry, 0, stk_size);
    thread_setprio(thread, prio);
}
