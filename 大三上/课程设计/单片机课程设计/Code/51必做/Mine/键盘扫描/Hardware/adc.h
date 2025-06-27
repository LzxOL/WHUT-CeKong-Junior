#ifndef _ADC_H_
#define _ADC_H_//写的源文件英文为小写这里也要用大写引用
#define uchar unsigned char	
#define uint  unsigned int
sbit CS   = P3^5;
sbit CLK  = P3^6;
sbit DIO = P3^7;
void DigDisplay();
unsigned char Get_AD_Result();
#endif
