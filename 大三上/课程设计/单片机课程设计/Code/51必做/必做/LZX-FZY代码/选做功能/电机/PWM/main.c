#include <reg51.h>
#include <intrins.h>//包含移位函数_crol_(  )的头文件

#define uchar unsigned char		  			
#define uint unsigned int
	
uchar timer=0;//timer记录中断次数
uchar flag=0;//记录电机状态


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
	TMOD=0x01;   	//设置T0为方式1
	ET0 = 1;		//允许定时器T0中断
	EA = 1;			//总中断允许
	TH0=65490/256;		//置T0高8位初值，单次定时0.05ms即50us
	TL0=65490%256;		//置T0低8位初值
	TR0=1;		  //接通T0
	  
	while(1)
	{

	if(flag == 0)
	{
		motorPin1 = 1;  // 设置电动机引脚1为高电平
		motorPin2 = 0;  // 设置电动机引脚2为低电平
		delay(3000);	
		flag = 1;
	}
	if(flag == 1)
	{
		motorPin1 = 0;  // 设置电动机引脚1为高电平
		motorPin2 = 0;  // 设置电动机引脚2为低电平
	}
}
		
  }
  
void int_T0() interrupt 1  using 0 		//定时器T0中断函数
{
	TR0=0;		 	//停止计时，执行以下操作（会带来计时误差）
	TH0=65490/256;		//置T0高8位初值，单次定时0.5ms即500us,2kHz
	TL0=65490%256;		//置T0低8位初值
	timer++;   	   	//记录中断次数
	 if (timer==2) //中断1次，共计时1*0.05ms=0.05ms  
     {  
       P2_0=!P2_0; // P2.0状态求反 
				 
     } 
	if (timer==30) //中断20次，共计时20*0.05ms=1ms 1kHz  
     {  
       P2_0=!P2_0; // P2.0状态求反 
	   timer=0; //中断次数清0
     } 
}