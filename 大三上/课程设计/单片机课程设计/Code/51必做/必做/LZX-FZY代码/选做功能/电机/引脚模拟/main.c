#include <reg51.h>

#define uchar unsigned char		  			
#define uint unsigned int
	
sbit P2_0=P2^0;		//定义特殊功能寄存器P2的位变量P2_0为PWM生成口
sbit motorPin1=P2^2;//定义特殊功能寄存器P2的位变量P2_2为电机引脚1
sbit motorPin2=P2^1;//定义特殊功能寄存器P2的位变量P2_1为电机引脚2

void delay(uint i)//延时函数
{	
	uchar t;
 	while (i--)	
		for(t=0;t<120;t++);
}

void main(void)		//主程序
  { 
	while(1)
	{
		P2_0 = 1;
		motorPin1 = 1;  // 设置电动机引脚1为高电平
		motorPin2 = 0;  // 设置电动机引脚2为低电平
		delay(3000);
		P2_0 = 0;
		motorPin1 = 0;  // 设置电动机引脚1为高电平
		motorPin2 = 0;  // 设置电动机引脚2为低电平
		delay(3000);
	}	
 }