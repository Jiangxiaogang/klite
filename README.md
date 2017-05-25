KLite说明文档
=====================
KLite是免费开源软件,基于 MIT 协议开放源代码.

作者: 蒋晓岗\<kerndev@foxmail.com> 保留所有权利.

# 一.简介
KLite是一个为ARM Cortex-M微控制器设计的微内核,设计思想是"简洁易用".  
它最大的特性在于简洁易用,可能是目前为止最简单易用的嵌入式操作系统内核.  
简洁的API风格,简洁的调用方式,简单的移植方法,大大降低学习难度,即使是初学  
者也能很容易开始进入多线程的开发方式.目前只实现一些最基本,但最常用的功能,  
如线程管理,内存管理,线程同步等.

# 二.移植
KLite已经为Cortex-M0/M3/M4构架做好了适配,如STM32F0/1/2/3/4系列单片机.  
如果你的CPU平台是基于以上三个平台的,那么可以直接使用预编译的库文件;  
如果不是则需要参考源代码自行移植CPU底层的汇编代码.

# 三.开始使用
## 1.编译KLite
	在build目录下面有预设的工程文件，选择你要使用的编译器和目标CPU平台,  
	编译完成后，会生成kernel.lib文件，将kernel.lib, kernel.h, cpu_os.c复制到你的项目源码中，  
	使用lib文件可以减少重复编译时间.

## 2.修改cpu_os.c
	根据目标CPU的编程手册，实现cpu_os.c里面的3个空函数.

## 3.在main函数里面添加初始化代码
	main函数的推荐写法如下:
```
void main(void)
{
	kernel_init(RAM_ADDR, RAM_SIZE);
	kthread_create(init, 0, 0);
	kernel_start();
}
```
说明:
kernel_init 用于初始化KLite,并设置可用内存;  
kthread_create 创建第一个线程init;  
kernel_start 用于启动KLite;  
init是一个线程函数，在该函数中实现你的其它初始化代码.  
相关函数参数说明请参照API文档.

# 四.支持
如果你在使用中发现任何BUG,请加入我的QQ群或者发邮件至kerndev@foxmail.com.  
目前没有Cortex-M7的器件,希望有人能提交基于CM7的代码.非常感谢!
