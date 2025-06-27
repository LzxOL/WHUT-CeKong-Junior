#include <REGX52.H>
#include <INTRINS.H>
#include "delay.h"

extern unsigned char code ShowNum[];
/*
数码表编码对应关系：
  0: 0xC0
  1: 0xF9
  2: 0xA4
  3: 0xB0
  4: 0x99
  5: 0x92
  6: 0x82
  7: 0xF8
  8: 0x80
  9: 0x90
a-f:0x88,0x83,0xc6,0xa1,0x86,0x8e
*/

extern unsigned int temp[3];
void InitUART(void) {
    PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFD;			//设定定时初值
	TH1 = 0xFD;			//设定定时器重装值		
	TR1 = 1;			//启动定时器1
	ET1 = 0;        	//禁止定时器1中断
	EA=1;				//开放总中断 
	ES=1;				//开放串口中断
}

void weichoice(unsigned int i, unsigned int j) {
    P3_7 = 0;
    P3_6 = 0;
    if (i > 7) i = i % 8;
    P2 = _crol_(0x01, i);
    P3_7 = 1;
    P3_7 = 0;
    P2 = ShowNum[j];
    P3_6 = 1;
    P3_6 = 0;
}

void tempcount(unsigned int k) {
    // 确保 k 在 0 - 99 范围内，拆分百、十、个位
    temp[0] = k / 100;  // 百位
    temp[1] = (k / 10) % 10; // 十位
    temp[2] = k % 10;  // 个位
}

void Delay(unsigned int i) {
    unsigned int j;
    for (; i > 0; i--) {
        for (j = 0; j < 125; j++) {}
    }
}

//void main(void) {
//    InitUART();
//    SendStr("Hello");

//    ES = 1; //允许串行口中断
//    while (1) {
//        unsigned int a;
//        for (a = 0; a < 3; a++) {
//            weichoice(a, temp[a]);
//            Delay(2);
//        }
//    }
//}

void SendByte(unsigned char dat) {
    SBUF = dat;
    while (TI == 0);
    TI = 0;
}

void SendStr(unsigned char *s) {
    while (*s != '\0') { // 发送字符串
        SendByte(*s);
        s++;
    }
}

//void UART_SER(void) interrupt 4 {
//    unsigned char Temp;
//    unsigned char temp0;

//    if (RI) {
//        RI = 0;
//        Temp = SBUF; // 接收到的数据
//        SBUF = Temp; // 串口回显
//        temp0 = Temp; // 临时存储接收到的字节
//        tempcount(temp0); // 更新 temp 数组
//    }
//    if (TI) {
//        TI = 0;
//    }
//}
