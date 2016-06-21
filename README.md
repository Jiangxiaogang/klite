#KLite说明文档
KLite是免费开源软件,基于LGPL协议开放源代码,建议以库文件的方式直接使用KLite.

蒋晓岗<kerndev@foxmail.com> 保留所有权利.

#一.简介
KLite是一个为ARM Cortex-M微控制器设计的微内核,设计思想是"简洁易用".

它最大的特性在于简洁易用,可能是目前为止最简单易用的嵌入式操作系统内核.

简洁的API风格,简洁的调用方式,简单的移植方法,大大降低学习难度,即使是初学

者也能很容易开始进入多线程的开发方式.目前只实现一些最基本,但最常用的功能,

如线程管理,内存管理,线程同步等.

#二.移植
KLite已经为Cortex-M构架做好了适配,如STM32F/1/2/3/4系列单片机.

如果你的CPU平台是基于以上三个平台的,那么可以直接使用预编译的库文件;

如果不是则需要参考源代码自行移植CPU底层的汇编代码.


#三.开始使用
##1.准备KLite库文件
	预编译库包含三个文件:
	kernel.lib 内核库文件(由kernel.c,cpu_core_cm3/4.c编译)
	kernel.h   内核头文件
	cpu_init.c CPU初始化代码
建议:在你的工程目录下新建'kernel'文件夹,添加KLite的三个文件,并添加至你的

工程中,根据情况修改cpu.c里面的CPU主频为实际的值.
##2.在main函数里面添加初始化代码:
```
void main(void)
{
	kernel_init(RAM_ADDR,RAM_SIZE);
	kthread_create(init,0,0);
	kernel_start();
}
```
注解:
kernel_init用于初始化KLite,并设置可用内存;

kthread_create创建第一个线程init,在init线程中执行你的初始化代码;

kernel_start用于启动KLite;

相关函数参数说明请参照API文档.

#四.支持
如果你在使用中发现任何BUG,请发邮件至kerndev@foxmail.com,我将尽力改进.

目前没有Cortex-M0和M7的器件,希望有人能提交基于CM0和CM7的Pull request.非常感谢!

