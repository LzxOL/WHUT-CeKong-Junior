#include "reg52.h"
#include "delay.h"
#include "shumaguan.h"

#define uchar unsigned char 
	
unsigned int time;//定义延迟时间

uchar dis_shumaguan[10]=//共阳极数码管字符0-9
{0xc0,0xf9,0xa4,
0xb0,0x99,0x92,
0x82,0xf8,0x80,0x90};

unsigned int h=9, m=59, s=50;
//  定义时,分,秒初始值，可直接更改，数码管从这个时间开始
unsigned int count=0;// 计时器中断中用到的变量


void Display() {     
    unsigned int h0 = 0, h1 = 0, m0 = 0, m1 = 0, s0 = 0, s1 = 0;  
    // 定义变量：h0 = 时的十位, h1 = 时的个位,
    // m0 = 分的十位, m1 = 分的个位, s0 = 秒的十位, s1 = 秒的个位
    
    // 计算时、分、秒的各个位
    h0 = h / 10;   // 小时除以10，得小时的十位数
    h1 = h % 10;   // 小时除以10取余，得小时的个位数
    m0 = m / 10;   // 分钟除以10，得分钟的十位数
    m1 = m % 10;   // 分钟除以10取余，得分钟的个位数
    s0 = s / 10;   // 秒钟除以10，得秒钟的十位数
    s1 = s % 10;   // 秒钟除以10取余，得秒钟的个位数
    
    // 显示“10” (日期)
    P3 = 0xff;     // 解锁
    P2 = 0x01;     // 开启 WX0
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[1];  // 显示数字 1
    delay_ms(2);   // 延时

    P3 = 0xff;     // 解锁
    P2 = 0x02;     // 开启 WX1
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[0];  // 显示数字 0
    delay_ms(2);   // 延时
    
    // 显示时间（小时）
    P3 = 0xff;     // 解锁
    P2 = 0x04;     // 开启 WX2
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[h0];  // 显示小时的十位数
    delay_ms(2);   // 延时
    
    P3 = 0xff;     // 解锁
    P2 = 0x08;     // 开启 WX3
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[h1];  // 显示小时的个位数
    delay_ms(2);   // 延时
    
    // 显示时间（分钟）
    P3 = 0xff;     // 解锁
    P2 = 0x10;     // 开启 WX4
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[m0];  // 显示分钟的十位数
    delay_ms(2);   // 延时
    
    P3 = 0xff;     // 解锁
    P2 = 0x20;     // 开启 WX5
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[m1];  // 显示分钟的个位数
    delay_ms(2);   // 延时
    
    // 显示时间（秒钟）
    P3 = 0xff;     // 解锁
    P2 = 0x40;     // 开启 WX6
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[s0];  // 显示秒钟的十位数
    delay_ms(2);   // 延时
    
    P3 = 0xff;     // 解锁
    P2 = 0x80;     // 开启 WX7
    P3 = 0x7f;     // 锁存
    P2 = dis_shumaguan[s1];  // 显示秒钟的个位数
    delay_ms(2);   // 延时
}


void shumaguan_init(){
	TMOD =0x01;
	TH0 = 0x3C;  // 初值高8位就是65536-计时值50000（50毫秒）
				 //= 15536转换成16进制为 3cb0
	TL0 = 0xB0;  // 高8位为3c， 低8位为b0
	ET0 =1;      // 开启定时器0
	EA = 1;      // 开启总中断
	TR0 = 1;     // 开启定时器
	
}
//void main()
//{
//	TMOD =0x01;
//	TH0 = 0x3C;  // 初值高8位就是65536-计时值50000（50毫秒） = 15536转换成16进制为 3cb0
//	TL0 = 0xB0;  // 高8位为3c， 低8位为b0
//	ET0 =1;      // 开启定时器0
//	EA = 1;      // 开启总中断
//	TR0 = 1;     // 开启定时器
//		
//	while(1)
//	{
//		Display();
//	}	
//}