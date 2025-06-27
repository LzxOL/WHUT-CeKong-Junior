
#include "reg52.h"

#define uchar unsigned char		  			
#define uint unsigned int
	
sbit sg90_con = P2^3;
 
uint jd;
uint cnt = 0;

void delay(uint i)//延时函数
{	
	uchar t;
 	while (i--)	
		for(t=0;t<120;t++);
}
 
void Time0Init()
{
	
	TMOD = 0X01;
	
	TH0 = 0XFE;
	TL0 = 0X33;
	
	TR0 = 1;
	TF0 = 0;
	
	ET0 = 1;
	
	EA = 1;
	
}
void main()
{
	jd = 1;  
	Time0Init();  
	sg90_con = 1;  
	
	while(1)
	{
		jd = 3; 
		cnt = 0;
		delay(2000);
		jd = 1; 
		cnt = 0;
		delay(2000);
	}
}
 
void Time0Handler() interrupt 1
{
	cnt++;  
	
	TH0 = 0XFE;
	TL0 = 0X33;
	if(cnt < jd)   
	{
		sg90_con = 1; 
	}else{
		sg90_con = 0;		
	}
	
	if(cnt == 40)   
	{
		cnt = 0;
		sg90_con = 1;
	}
}