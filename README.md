#klite
KLite说明文档
日期: 2016.6.10
作者: 蒋晓岗<kerndev@foxmail.com>

一.简介
KLite是专为嵌入式系统平台开发的一个简洁易用的操作系统内核.它最大的特性在于简洁易
用,可能是目前为止最简单易用的嵌入式操作系统内核.简洁的API风格,简洁的调用方式,简
单的移植方法,大大降低学习难度,即使是初学者也能很容易开始进入多线程的开发方式.目
前只实现一些最基本,但都是最常用的功能,如线程管理内存管理,线程同步等.

二.移植
KLite已经为Cortex-M0/M3/M4构架做好了适配,如STM32F0/1/2/3/4系列单片机.如果你的CPU
平台是基于以上三个平台的,那么可以直接使用预编译的库文件;如果不是则需要参考源代码
自行移植CPU底层的汇编代码.

三.开始使用
1.准备KLite库文件
	预编译库包含三个文件:
	kernel.lib 内核库文件
	kernel.h   内核头文件
	cpu_init.c CPU初始化代码,用于启动滴答定时器
建议:在你的工程目录下新建'kernel'文件夹,并复制KLite的三个文件,然后添加至你的工程
中,最后根据情况修改cpu.c里面的CPU主频为实际的值.
2.在main函数里面添加初始化代码:
void main(void)
{
	kernel_init(RAM_ADDR,RAM_SIZE);
	kthread_create(init,0,0);
	kernel_start();
}
注解:
kernel_init用于初始化KLite,并设置可用内存;
kthread_create创建第一个线程init,在init线程中执行你的初始化代码;
kernel_start用于启动KLite;
相关函数参数说明请参照第四章节.

四.API参考
待添加!