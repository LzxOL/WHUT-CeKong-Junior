#include "reg52.h"
#include "adc.h"
#include <stdio.h>
#include "delay.h"

extern unsigned char AD1;

extern unsigned char dis_adc[10];
extern unsigned int i,j;
unsigned char date[8];//ADC0832


void DigDisplay()
{
    int i;
    for(i=0; i<8; i++)
    {
        switch(i)
        {
            case(0):
                P3 = 0xff;  // 解锁
                P2 = 0x01;  // 开启WX0
                P3 = 0x7f;  // 锁存
                P2 = date[i] & 0x7f;
                delay_ms(2);  // 增加延时，减少闪烁速度
                break;

            case(1):
                P3 = 0xff;  // 解锁
                P2 = 0x02;  // 开启WX1
                P3 = 0x7f;  // 锁存
                P2 = date[i];
                delay_ms(2);  // 增加延时，减少闪烁速度
                break;

            case(2):
                P3 = 0xff;  // 解锁
                P2 = 0x04;  // 开启WX2
                P3 = 0x7f;  // 锁存
                P2 = date[i];
                delay_ms(2);  // 增加延时，减少闪烁速度
                break;

            case(3):
                P3 = 0xff;  // 解锁
                P2 = 0x08;  // 开启WX3
                P3 = 0x7f;  // 锁存
                P2 = date[i];
                delay_ms(2);  // 增加延时，减少闪烁速度
                break;
        }
                delay_ms(1);  // 增加延时，减少闪烁速度
    }
}

unsigned char Get_AD_Result()
{
    uchar i;
    uchar data1=0,data2=0;
    CS=0;
    
  //第一个下降沿到来前，DI需置1，起始控制位，开始转换
    CLK=0;DIO=1;    
    CLK=1;
    
 //第二个下降沿到来前，设D=1/0，选择单端/差分（SGL/DIF）模式中的单端输入模式
    CLK=0;DIO=1;   
    CLK=1;           
 
 //第三个下降沿到来前，设D=0/1，选择CH0/CH1，这里选择单通道ch0  
    CLK=0;DIO=1;    
    CLK=1;DIO=0;   
    
 //第四个下降沿到来前,DI =1 
    CLK=0;DIO=1; 
    
//4-11，共8个下降沿 DO输出转换信号，读取数据（MSB-->LSB） 
    for(i=0;i<8;i++)
    {
        CLK=1;
        CLK=0;
       data1=(data1<<1)|(uchar)DIO;
    }
    CS=1;
    return data1;
}

//void main(void)
//{
//	
//	uint vt1;	
//	uint while_times = 0;
//	
//	delay_ms(300);
//	ET0  = 1;
//    TR0  = 1;
//    ET1  = 1;
//    TR1  = 1;
//    EA   = 1;     
//    CLK  = 0;      
//    CS   = 1; 
//	 
//	while(1)
//	{	
//	   AD1=Get_AD_Result();
//	   vt1=	(5000/256.0)*AD1;
//		date[0] =dis_adc[vt1/1000];
//		date[1] =dis_adc[vt1/100%10]; 
//		date[2] =dis_adc[vt1%100/10];
//		date[3] =dis_adc[vt1%10];
//		DigDisplay(); 	
//	}
//}