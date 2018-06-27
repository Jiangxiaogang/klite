/*
* KLite示例工程
*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "log.h"
#include "gpio.h"
#include "timer.h"
#include "mbox.h"

static timer_t m_timer;
static event_t m_event;
static mbox_t  m_mbox;

//定时器处理函数
static void timer_handler(void *arg)
{
	LOG("I am timer handler(time=%u, arg=0x%p)\r\n", kernel_time(), arg);
}

static void demo1_thread(void *arg)
{
    bool ret;
    uint32_t data;
    LOG("demo1_thread: 0x%08X\r\n", thread_self());
    while(1)
    {
        ret = mbox_timedwait(m_mbox, 2000, &data);
        LOG("demo1_thread: timedwait = %s\r\n", ret ? "true" : "false");
    }
}

//LED闪烁线程1
static void blink_thread1(void *arg)
{
	LOG("blink_thread1: 0x%08X\r\n", thread_self());
	gpio_open(PC, 10, GPIO_MODE_OUT, GPIO_OUT_PP);
	while(1)
	{
		gpio_write(PC, 10, 1);
		thread_sleep(200);
		gpio_write(PC, 10, 0);
		thread_sleep(200);
	}
}

//LED闪烁线程2
static void blink_thread2(void *arg)
{
	LOG("blink_thread2: 0x%08X\r\n", thread_self());
    thread_setprio(thread_self(), -1);
	gpio_open(PG, 11, GPIO_MODE_OUT, GPIO_OUT_PP);
	while(1)
	{
		gpio_write(PG, 11, 1);
		thread_sleep(500);
		gpio_write(PG, 11, 0);
		thread_sleep(500);
        mbox_post(m_mbox, 1);
        LOG("blink_thread2: posted\r\n");
        event_set(m_event);
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
    timer_init(1024, 0);
    m_timer = timer_create();
    m_event = event_create(0, 0);
    m_mbox  = mbox_create();
    timer_start(m_timer, 5000, timer_handler, 0);
	thread_create(usage_thread, 0, 0);
    thread_create(demo1_thread, 0, 0);
	thread_create(blink_thread1, 0, 0);
    thread_create(blink_thread2, 0, 0);
}

//初始化线程
void init(void *arg)
{
  	bsp_init();
	app_init();
}

int main(void)
{
	static uint8_t heap[8*1024];
	kernel_init((uint32_t)heap, 8*1024);
    //tasklet_init(1024); //tasklet必须初始化才能使用
	thread_create(init, 0, 0);
	kernel_start();
}
