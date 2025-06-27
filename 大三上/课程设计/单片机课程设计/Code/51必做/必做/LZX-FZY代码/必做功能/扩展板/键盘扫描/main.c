#include "reg52.h"

#define uchar unsigned char 
	
unsigned int time;//定义延迟时间

sbit R1=P1^5;//定义键盘的3行线
sbit R2=P1^4;
sbit R3=P1^3;

uchar dis[16]=//共阳极数码管字符1-9
{0xf9,0xa4,0xb0,
0x99,0x92,0x82,
0xf8,0x80,0x90};

void delay(time)//延时函数
{   
	unsigned int j; 
    for(j=0;j<time;j++)
    {}
}

void main()
{
    uchar temp;//中间变量
    uchar i;//计算变量
	P3=0xff;//解锁
	P2=0x01;//开启WX0
	P3=0x7f;//锁存
	P2=0xff;
   
	while(1)
	{
		P1=0xfb;//列扫描初值1111 1011，使p1.2为0,其余为1
        for(i=0;i<=2;i++)//按列扫描，i为列变量，共3行
        {
          if(R1==0)
		  {
			  delay(20);
			  P2=dis[i];//1,2,3
		  }
          if(R2==0)
		  {
              delay(20);
			  P2=dis[i+3];//4,5,6
		  } 			  
          if(R3==0)
		  {
			  delay(20);
              P2=dis[i+6];//7,8,9
		  }			  
            
          delay(500);//延时
          temp = P1;//读入P1口状态
          temp = temp|0xf0;//置p1.0-1.1为1，作输入
          temp = temp>>1;//左移向下一列扫描
          temp = temp|0xf0;//置p1.0-1.1为1，作输入
          P1 = temp;//为下一次循环做准备
        }
	}
}