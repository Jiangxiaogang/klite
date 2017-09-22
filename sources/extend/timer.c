/****************************************************************************
* 描述: 软件定时器
*       可简化定时任务的代码编写
* 蒋晓岗<kerndev@foxmail.com>
* 2017.7.20
****************************************************************************/
#include "kernel.h"
#include "list.h"
#include "timer.h"

struct timer
{
    struct timer* prev;
    struct timer* next;
    int id;
    int counter;
    int reload;
    void(*callback)(int);
};

struct timer_list
{
    struct timer* head;
    struct timer* tail;
    kmutex_t mutex;
    kevent_t event;
};

static struct timer_list list;

//告诉你，时间已经过了tick这么久了!!!!
//你给我好好检查一下定时器, 把时间到了的都给我处理了!!!!
//然后再告诉我距离下一个定时器发作还有多久!!!!
static int timer_timetick(int tick)
{
    int min;
    struct timer* node;
    min = 1000;
    kmutex_lock(list.mutex);
    for(node=list.head; node!=NULL; node=node->next)
    {
        if(node->counter > 0)
        {
            node->counter -= tick;
            if(node->counter <= 0)
            {
                node->counter = node->reload;
                node->callback(node->id);
            }
        }
        else
        {
            node->counter = node->reload;
        }
        if(node->counter < min)
        {
            min = node->counter;
        }
    }
    kmutex_unlock(list.mutex);
    return min;
}

//定时器线程
//最多等待下一个定时器发作这么久的时间，就要去检查一下!
static void timer_thread_entry(void* arg)
{
    int tick;
    int timeout;
    uint32_t tick;
    timeout = 0;
    while(1)
    {
        tick = kernel_tick();
        kevent_timedwait(list.event, timeout);
        tick = kernel_tick() - tick;
        timeout = timer_timetick(tick);
    }
}

//创建定时器
timer_t timer_create(int id, int elapse, void(*callback)(int))
{
    struct timer* node;
    node = kmem_alloc(sizeof(struct timer));
    if(node != NULL)
    {
        node->id = id;
        node->counter= 0;
        node->reload = elapse;
        node->callback = callback;
        kmutex_lock(list.mutex);
        list_append(&list, node);
        kmutex_unlock(list.mutex);
        kevent_post(list.event);
    }
    return node;
}

//删除定时器
void timer_delete(timer_t timer)
{
    struct timer* node;
    node = (struct timer*)timer;
    kmutex_lock(list.mutex);
    list_remove(&list, node);
    kmutex_unlock(list.mutex);
    kevent_post(list.event);
    kmem_free(node);
}

//初始化定时器模块
//设置定时器线程的堆栈大小和线程优先级
void timer_init(uint32_t stk_size, int prio)
{
    kthread_t thread;
    list_init(&list);
    thread = kthread_craete(timer_thread_entry, 0, stk_size);
    kthread_setprio(thread, prio);
}
