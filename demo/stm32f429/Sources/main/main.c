/*
* KLite测试工程
*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "timer.h"
#include "semaphore.h"
#include "log.h"
#include "gpio.h"

static timer_t  m_timer;
static event_t  m_event;
static thread_t m_idle;
static sem_t    m_sem;

//获取系统空闲时长(现已从内核API中移除，所以自己实现)
uint32_t kernel_idletime(void)
{
    return thread_time(m_idle);
}

//定时器处理函数
static void timer_handler(void *arg)
{
	LOG("I am timer handler, time=%u, arg=0x%p.\r\n", kernel_time(), arg);
}

//演示线程
//定时发送信号
static void demo1_thread(void *arg)
{
    LOG("demo1_thread: 0x%08X\r\n", thread_self());
    while(1)
    {
        sleep(1000);
        LOG("demo1_thread wake up at %u\r\n", kernel_time());
        //event_post(m_event);
        sem_post(&m_sem);
    }
}

//LED闪烁线程1
//每闪烁一次LED灯，等待一次信号
static void blink_thread1(void *arg)
{
	LOG("blink_thread1: 0x%08X\r\n", thread_self());
	gpio_open(PC, 10, GPIO_MODE_OUT, GPIO_OUT_PP);
	while(1)
	{
        //sleep(250);
		gpio_write(PC, 10, 1);
        //sleep(250);
        sem_wait(&m_sem);
        LOG("blink_thread1 wake up at %u\r\n", kernel_time());
        
        
		gpio_write(PC, 10, 0);
        //event_wait(m_event);
        sem_wait(&m_sem);
        LOG("blink_thread1 wake up at %u\r\n", kernel_time());
	}
}

//LED闪烁线程2
//每闪烁一次LED灯，等待一次信号
static void blink_thread2(void *arg)
{
	LOG("blink_thread2: 0x%08X\r\n", thread_self());
	gpio_open(PG, 11, GPIO_MODE_OUT, GPIO_OUT_PP);
	while(1)
	{
        //sleep(500);
		gpio_write(PG, 11, 1);
        sem_wait(&m_sem);
        LOG("blink_thread2 wake up at %u\r\n", kernel_time());
        
		//sleep(500);
		gpio_write(PG, 11, 0);
        sem_wait(&m_sem);
        LOG("blink_thread2 wake up at %u\r\n", kernel_time());
	}
}

//计算资源占用率
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
	log_init();
    sem_init(&m_sem, 0);
    m_event = event_create(0);
    timer_start(&m_timer, 5000, timer_handler, 0);
	thread_create(usage_thread, 0, 384);
    thread_create(demo1_thread, 0, 384);
	thread_create(blink_thread1, 0, 384);
    thread_create(blink_thread2, 0, 384);
}

//初始化线程
void init(void *arg)
{
  	bsp_init();
	app_init();
}

void idle(void *arg)
{
    thread_setprio(thread_self(), THREAD_PRIORITY_MIN);
    while(1);
}

int main(void)
{
	static uint8_t heap[8*1024];
    kernel_init((uint32_t)heap, sizeof(heap));
    timer_init(1024, 0);
//    //tasklet_init(1024); //tasklet初始化
    thread_create(init, 0, 0);
    m_idle = thread_create(idle, 0, 0);
    kernel_start();
}
