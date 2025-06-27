#include <reg51.h>
#include <stdio.h>

#define uchar unsigned char		  			
#define uint unsigned int
	
uchar dis[10]=//共阳极数码管字符0-9
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar temp = 0;
uchar flag = 0;
uint i;

void delay(i)//延时函数
{	
	uchar t;
 	while (i--)	
		for(t=0;t<120;t++);	
}

void main(void)
 { 
	 TMOD = 0x20;//设置定时器T1为方式2
	 TH1 = 0xfd;//波特率9600
	 TL1 = 0xfd;
	 SCON = 0x50;//设置串口为方式1接收，REN=1
	 PCON = 0x00;//SMOD = 0
	 TR1 = 1;//启动T1
	 EA = 1;//总中断允许位
	 ES = 1;//打开接收中断
	 
	 P3=0xff;//解锁
	 P2=0x01;//开启WX0
	 P3=0x7f;//锁存
	 P2=0xff;
	 
	 while (1)
	{ 
		;		
	}
 }
 
void int1() interrupt 4
{
	if(RI)
	{
		temp = SBUF;
		RI = 0;	//清除接收中断标志位
		flag = 1;
		while(RI == 0)
		{
			P2 = dis[temp];
			if(flag==1)
			{
				//发送接收到字符
				SBUF=temp;
				while(!TI);
				TI=0;
				flag=0;
			}
			delay(10);
		}			
	}
}	