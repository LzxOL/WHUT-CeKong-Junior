#include "reg52.h"
#include "key.h"
#include "shumaguan.h"
#include "delay.h"
#include "lcd.h"
#include "adc.h"
#include "uart.h"

#include <intrins.h>

#define uchar unsigned char	
#define uint  unsigned int
unsigned int i,j;
unsigned char str1[]={"2025-1-13"};
unsigned char str2[]={"HelloWorld!imlzx"};
 

uchar dis[10]=//共阳极数码管字符0-9
{0xc0,0xf9,0xa4,
0xb0,0x99,0x92,
0x82,0xf8,0x80,0x90};


/* ADC 初始化参数*/
uchar code dis_adc[] = {
    0xf9, 0xa4, 0xb0,  // 1,2,3
    0x99, 0x92, 0x82,  // 4,5,6
    0xf8, 0x80, 0x90   // 7,8,9
};
uchar AD1;
uint vt1;	
uint while_times = 0;
extern unsigned char date[8];

/* 数码管动态显示 */
extern unsigned int h,m,s;
unsigned int h0=0,h1=0,m0=0,m1=0,s0=0,s1=0;  
unsigned int count_timer0=0;
/* 中断 */
sbit key_interrupt = P3^2;
sbit P3_7 = P3^7;
sbit P3_6 = P3^6;

unsigned int button_cnt = 0;
unsigned char code ShowNum[] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90, // 9
    0x88, // A
    0x83, // B
    0xC6, // C
    0xA1, // D
    0x86, // E
    0x8E  // F
};
unsigned int a;
unsigned int temp_str[3];

uint init_flag = 0;
// 主函数
void main(void) {
    uchar key = 0;
	uchar temp = 0;
  
	while(1) {
		if(!key){
			key = scan_keyboard();
		}
		switch(key) {// LCD
        case 1:
			if(!init_flag){
				init_flag = 1;
				lcd_init();
			}
			lcd_init();// 初始化

			xml(0x80);        // 从lcd的第一行左侧开始写入 1000 0000
			for(i=0;i<=8;i++)
			{
				xdat(str1[i]);
				delay_ms(5);
			}
			for(j=15;j>=0;j--)
			{
				xml(0xc0+j);  // 从lcd的第二行左侧开始写入
				for(i=0;i<=14;i++)
				{
					xdat(str2[i]);
					delay_ms(5);
				}
				delay_ms(500);
			}
			break;
			
        case 2: // adc
            // 按键2的功能
			if (!init_flag) {
				init_flag = 1;
				lcd_init();  // 初始化 LCD
			}

			// 显示第一行字符
			xml(0x80);  // 设置光标到 LCD 第一行左侧
			for (i = 0; i <= 8; i++) {
				xdat(str1[i]);  // 写入字符
				delay_ms(5);
			}

			// 获取 AD1 转换结果
			AD1 = Get_AD_Result();  // 获取 AD1 的采样值
			vt1 = (5000 / 256.0) * AD1;  // 转换为电压值（单位 mV）

			// 将转换后的电压值拆分为字符
			date[0] = (vt1 / 1000) + '0';      // 千位转换为字符
			date[1] = (vt1 / 100 % 10) + '0';  // 百位转换为字符
			date[2] = (vt1 % 100 / 10) + '0';  // 十位转换为字符
			date[3] = (vt1 % 10) + '0';        // 个位转换为字符

			// 显示 AD1 转换后的电压值在第二行
			xml(0xC0);  // 设置光标到 LCD 第二行左侧
			xdat('A');  // 显示 "A"
			xdat('D');  // 显示 "D"
			xdat('1');  // 显示 "1"
			xdat(':');  // 显示 ":"

			for (i = 0; i < 4; i++) {
				xdat(date[i]);  // 显示转换后的电压值
				delay_ms(5);
			}

			xdat('m');  // 显示 "m"
			xdat('V');  // 显示 "V"

			delay_ms(500);  // 延时

			break;
            
        case 3:
            // 按键3的功能 数码管shownum 
			if(!init_flag){
				init_flag = 1;
				TMOD =0x01;
				TH0 = 0x3C;  // 初值高8位就是65536-计时值50000（50毫秒） = 15536转换成16进制为 3cb0
				TL0 = 0xB0;  // 高8位为3c， 低8位为b0
				ET0 =1;      // 开启定时器0
				EA = 1;      // 开启总中断
				TR0 = 1;     // 开启定时器
				
			}
			Display();
            break;
        case 4: // 中断
			if(!init_flag){
				init_flag = 1;
				P2 = 0x01;  // 初始化P2口
	
				EA = 1; 
				IT0 = 1;  
				EX0 = 1;   
			}
			for(j = 0; j < 8; j++)
			{
				P2 = 0x01;  // 重新初始化P2，开始选通第一个数码管
				P2 = _crol_(P2, j);  // 循环左移，选通相应的数码管
				P3_7 = 1;  // 产生锁存信号，高电平
				P3_7 = 0;  // 锁存信号恢复为低电平

				P2 = ShowNum[button_cnt];  // 显示编码，输出到P2口
				P3_6 = 1;  // 产生显示信号，高电平
				P3_6 = 0;  // 显示信号恢复为低电平
				delay_ms(1);  // 短延时实现视觉暂留效果
			}
				
			break;
		case 5:
			if(!init_flag){
				init_flag = 1;
				InitUART();
				SendStr("Hello");

				ES = 1; //允许串行口中断
			}
			for (a = 0; a < 3; a++) {
				weichoice(a, temp_str[a]);
				Delay(2);
			}
			break;
        default:

			break;
    }

    }
}


void int0() interrupt 0 {
    if (!key_interrupt) {
        button_cnt += 1;
        button_cnt = button_cnt & 0x0F;  // 保证计数在0-15之间循环
        delay_ms(100);
    }
}

//  时间中断函数
void timer0() interrupt 1{
	unsigned int i=0;
	TH0 = 0x3C;     // 初值高8位就是65536-计时值50000（50毫秒） = 15536转换成16进制为 3cb0
	TL0 = 0xB0;     // 高8位为3c， 低8位为b0
	count_timer0++;		//  这个中断函数每隔50毫秒运行一次，每次count加1
	if(count_timer0==20){  //  如果count等于了20，就是运行了20次X50=1000毫秒=1秒   
		s+=1;	// 秒钟加1
		count_timer0 = 0;	// count置0重新计算
	}
	if(s==60){   //  秒钟=60
		m+=1;		//分钟加1
		s=0;		// 秒钟置0从新计算
	}
	if(m==60){    // 如果分钟等于60了
		h+=1;		// 小时加1
		m=0;		// 分钟置0从新计算
	}
	if(h==24){	//  如果小时等于24了或是12了
		h=0;		//  小时置0从新计算      （后面还可以加天，月， 年）
	}
}


void UART_ISR() interrupt 4//串口中断函数
{
	if(RI==1)//接收中断
	{
		SendByte(SBUF);//把接收的数据发送到计算机
		RI=0;//软件清零
	}
}
