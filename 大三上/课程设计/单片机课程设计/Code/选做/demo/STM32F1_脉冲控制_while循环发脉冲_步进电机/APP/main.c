#include "board.h"
#include "delay.h"

/**********************************************************
*** ZDT_X42_V2.0步进闭环控制例程
*** 编写作者：ZHANGDATOU
*** 技术支持：张大头闭环伺服
*** 淘宝店铺：https://zhangdatou.taobao.com
*** CSDN博客：http s://blog.csdn.net/zhangdatou666
*** qq交流群：262438510
**********************************************************/

/**
	*	@brief		MAIN函数
	*	@param		无
	*	@retval		无
	*/
int main(void)
{
	int32_t i = 0;

	// 初始化板载外设
	board_init();

	// 上电延时2秒等待ZDT_X42_V2闭环初始化完毕
	delay_ms(2000);

	// 现象：顺时针转一圈 -> 停1秒 -> 逆时针转一圈 -> 停1秒 -> 如此循环...
	while(1)
	{
		// 16细分下发送3200个脉冲，控制电机正转一圈
		// 脉冲频率（电机转速）通过延时函数delay_cnt()改变
		// 改小delay_cnt()的值转速变快，改大转速变慢
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);  	// 方向信号输出低电平，电机正转
		for(i=0; i < 800; i++)             	// 输出3200个脉冲信号
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_6);    // 输出高电平 
			delay_cnt(1000+i*20);           					// 高电平持续时间
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);	// 输出低电平 
			delay_cnt(1000+i*20);           					// 低电平持续时间
		}

		// 延时1秒
		delay_ms(1000);

		// 16细分下发送3200个脉冲，控制电机正转一圈
		// 脉冲频率（电机转速）通过延时函数delay_cnt()改变
		// 改小delay_cnt()的值转速变快，改大转速变慢
//		GPIO_SetBits(GPIOB, GPIO_Pin_7);    	// 方向信号输出高电平，电机反转
////		GPIO_ResetBits(GPIOB, GPIO_Pin_7);    	// 方向信号输出高电平，电机反转

//		for(i=0; i < 800; i++)             	// 输出3200个脉冲信号
//		{
//			GPIO_SetBits(GPIOB, GPIO_Pin_6);    // 输出高电平 
//			delay_cnt(1000+i*10);           					// 高电平持续时间
//			GPIO_ResetBits(GPIOB, GPIO_Pin_6);	// 输出低电平 
//			delay_cnt(1000+i*20);           					// 低电平持续时间
//		}

//		// 延时1秒
//		delay_ms(1000);
	}
}
