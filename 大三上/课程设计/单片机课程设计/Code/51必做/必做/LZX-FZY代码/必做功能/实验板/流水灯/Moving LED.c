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
   	P0=0xfe;//向P0口送出点亮数据11111110
   	while (1)	
   	{  	
		delay( 500 );//500为延时参数，可根据实际需要调整
		P0=_crol_(P0,1) ;// 函数_crol_(P0,1)把P0中的数据循环左移1位
		if(P0 == 0xf7)//如果P0为11110111
		{
			while(1)
			{
				delay( 500 );//500为延时参数，可根据实际需要调整
				P0=_cror_(P0,1) ;// 函数_cror_(P0,1)把P0中的数据循环右移1位
				if(P0 == 0xfe)
					break;
			}
		}
   	}
}