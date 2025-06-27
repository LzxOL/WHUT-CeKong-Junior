
#include "reg52.h"

#define uchar unsigned char		  			
#define uint unsigned int
	
sbit P3_2=P3^2;		//定义特殊功能寄存器P2的位变量P2_0为PWM生成口
sbit P0_0=P0^0;		//定义特殊功能寄存器P2的位变量P2_0为PWM生成口

/*void delay(uint i)//延时函数
{	
	uchar t;
 	while (i--)	
		for(t=0;t<120;t++);
}*/
 
void main()
{  
	while(1)
	{
		if(P3_2 == 1)
			P0_0 = 1;
		else
			P0_0 = 0;
	}
}
 