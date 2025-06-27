#include <reg51.h>
#include <intrins.h>//包含移位函数_crol_(  )的头文件

#define uchar unsigned char		  			
#define uint unsigned int
	
void delay(uint i)//延时函数
{	
	uchar t;
 	while (i--)	
		for(t=0;t<120;t++);	
}

void main()//主程序
{	
   	while (1)	
   	{  	
		P0=0xfe;//向P0口送出点亮数据11111110
		delay(500);//延时0.5s
		P0=0xfd;//向P0口送出点亮数据11111101
		delay(500);//延时0.5s
		P0=0xfb;//向P0口送出点亮数据11111011
		delay(500);//500为延时参数，可根据实际需要调整
		P0=0xf7;//向P0口送出点亮数据1110111
		delay(500);//500为延时参数，可根据实际需要调整
		P0=0xfb;//向P0口送出点亮数据11111011
		delay(500);//500为延时参数，可根据实际需要调整
		P0=0xfd;//向P0口送出点亮数据11111101
		delay(500);//延时0.5s
   	}
}