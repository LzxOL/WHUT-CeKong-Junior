#include "reg52.h"
#include "delay.h"
#include "lcd.h"

sbit RS=P1^0;      // lcd的RS接口接到单片机的P1的第0口
sbit RW=P1^1;		// lcd的RW接口接到单片机的P1的第1口
sbit E=P1^2;		// lcd的E接口接到单片机的P1的第2口

//  在lcd显示时用到的字符串，最后一个是时间间隔符：，也可以换成-号
extern unsigned char str1[];
extern unsigned char str2[];
extern unsigned int i,j;
//  lcd写数据函数 写dat首字母
void xdat(unsigned char dat)
{
	RS = 1;
	RW = 0;
	E = 0;
	P0 = dat;
	delay_ms(5);
	E = 1;
	E = 0;
}
 
//  lcd写命令函数  写命令首字母xml
void xml(unsigned char ml){
	RS = 0;
	RW = 0;
	E = 0;
	P0 = ml;
	delay_ms(5);
	E = 1;
	E = 0;
}
 
//  lcd初始化函数
void lcd_init(){
	xml(0x38);//八位数据接口，两行显示，5*7点阵
	xml(0x0c);//显示开，光标关，闪烁关
	xml(0x06);//数据读写操作后，光标自动加一，画面不动
	xml(0x01);//光标复位，清屏
}
 
// 主函数
void delay_lcd(){
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
}