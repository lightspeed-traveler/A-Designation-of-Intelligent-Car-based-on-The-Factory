/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 ********************************************************************************************************************/


#include "headfile.h"
#include "my_functions.h"
#pragma section all "cpu0_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

//extern int16 num_dis;



//工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
//工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
//然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
//一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

//对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用enableInterrupts();来开启中断嵌套
//简单点说实际上进入中断后TC系列的硬件自动调用了disableInterrupts();来拒绝响应任何的中断，因此需要我们自己手动调用enableInterrupts();来开启中断的响应。
int core0_main(void)
{
	get_clk();//获取时钟频率  务必保留
	//用户在此处调用各种初始化函数等

	init_motor();//电机初始化
	servo_init();//舵机初始化
	init_encoder();//编码器初始化，必须插上，不然程序会卡在这
    lcd_init();
    uart_init(UART_0, 115200, UART0_TX_P14_0, UART0_RX_P14_1);
    seekfree_wireless_init();
    systick_start(STM0);            //使用STM1 进行计时
//    uart_init(UART_2, 115200, UART2_TX_P10_5, UART2_RX_P10_6);
    //等待所有核心初始化完毕
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
	enableInterrupts();

	//注意 从V1.1.6版本之后  printf打印的信息从串口输出具体可以学习库例程Printf_Demo
    //注意 从V1.1.6版本之后  printf打印的信息从串口输出具体可以学习库例程Printf_Demo
    //注意 从V1.1.6版本之后  printf打印的信息从串口输出具体可以学习库例程Printf_Demo
	while (TRUE)
	{

		//用户在此处编写任务代码
	   global_control();
//	    motor_out(-3000);
	   // get_data();


	}
}

#pragma section all restore


