/*
* KLite测试工程
*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "semaphore.h"
#include "log.h"
#include "gpio.h"

static sem_t    m_sem;
static event_t  m_event;
static event_t  m_event2;
static thread_t m_idle;

//获取系统空闲时长(现已从内核API中移除，所以自己实现)
uint32_t kernel_idletime(void)
{
    return thread_time(m_idle);
}

//演示线程
//测试线程自动退出
static void demo1_thread(void *arg)
{
    LOG("demo1_thread: 0x%08X\r\n", thread_self());
    event_wait(m_event);    //线程同步，等待通知
    LOG("demo1_thread: 0x%08X exited!\r\n", thread_self());
    sem_post(&m_sem);
}

//LED闪烁线程
//测试线程休眠时间
static void blink_thread(void *arg)
{
	LOG("blink_thread1: 0x%08X\r\n", thread_self());
	gpio_open(PC, 10, GPIO_MODE_OUT, GPIO_OUT_PP);
    gpio_open(PG, 11, GPIO_MODE_OUT, GPIO_OUT_PP);
	while(1)
	{
		gpio_write(PC, 10, 1);
        gpio_write(PG, 11, 1);
        sleep(500);
		gpio_write(PC, 10, 0);
        gpio_write(PG, 11, 0);
        sleep(500);
        //LOG("%u\r\n", kernel_time());
	}
}

//计算资源占用率
//利用“空闲时间/总时间”计算CPU使用率
static void usage_thread(void *arg)
{
	uint32_t ver;
	uint32_t tick;
	uint32_t idle;
	uint32_t used;
	uint32_t total;
	
	ver = kernel_version();
	heap_usage(&total, &used);
	LOG("KLite V%d.%d.%d\r\n", (ver>>24)&0xFF, (ver>>16)&0xFF, ver&0xFFFF);	
	LOG("memory: %d/%d Bytes\r\n", used, total);
	LOG("thread: 0x%08X\r\n", thread_self());
	
	while(1)
	{
		tick = kernel_time();
		idle = kernel_idletime();
		sleep(1000);
		tick = kernel_time() - tick;
		idle = kernel_idletime() - idle;
		heap_usage(&total, &used);
		LOG("CPU:%2d%%, RAM:%d/%dB\r\n", 100*(tick-idle)/tick, used, total);
        event_wakeone(m_event2);    //如果有线程在等待，则唤醒其中一个
	}
}

void bsp_init(void)
{
	gpio_init(PA);
	gpio_init(PB);
	gpio_init(PC);
    gpio_init(PD);
    gpio_init(PE);
    gpio_init(PF);
    gpio_init(PG);
}

void app_init(void)
{
    int cnt;
    thread_t thrd;
    cnt = 0;
	log_init();
    sem_init(&m_sem, 0);
    m_event = event_create();
    m_event2= event_create();
	thread_create(usage_thread, 0, 0);
	thread_create(blink_thread, 0, 0);
    sleep(10000);
    while(1)
    {
        event_wait(m_event2);   //等待打印内存用量
        thrd = thread_create(demo1_thread, 0, 0);
        if(thrd == NULL)
        {
            LOG("create threads count: %d.\r\n", cnt); 
            while(cnt)
            {
                event_wait(m_event2);   //等待打印内存用量
                event_post(m_event);    //触发事件，通知等待中的线程
                sem_wait(&m_sem);       //等待计数信号量
                cnt--;
            }
            LOG("all threads exit.\r\n");
            sleep(10000);
            continue;
        }
        cnt++;
    }
}

//初始化线程
void init(void *arg)
{
  	bsp_init();
	app_init();
}

//空闲线程
void idle(void *arg)
{
    thread_t thrd;
    thrd = thread_self();
    LOG("idle thread: 0x%08X\r\n", thrd);
    thread_setprio(thrd, THREAD_PRIORITY_IDLE); //设置线程优先级为空闲级
    while(1)
    {
        thread_cleanup();   //清理已退出的线程(必须调用,否则自动退出的线程资源不会被释放)
    }
}

int main(void)
{
	static uint8_t heap[16*1024];
    kernel_init((uint32_t)heap, sizeof(heap));
    thread_create(init, 0, 0);
    m_idle = thread_create(idle, 0, 0);
    kernel_start();
}
