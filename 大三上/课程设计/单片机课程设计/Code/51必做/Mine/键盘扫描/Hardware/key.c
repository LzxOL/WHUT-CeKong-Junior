#include "key.h"
#include "reg52.h"
#include "delay.h"

// 引脚定义
sbit R1 = P1^5;    // 定义键盘的3行线
sbit R2 = P1^4;
sbit R3 = P1^3;

// 共阳极数码管显示字符1-9

// 初始化显示设置
void init_display(void) {
    P3 = 0xff;    // 解锁
    P2 = 0x01;    // 开启WX0
    P3 = 0x7f;    // 锁存
    P2 = 0xff;
}

// 矩阵键盘扫描函数，返回按下的键值（1-9），如果没有按键返回0
uchar scan_keyboard(void) {
    uchar temp;
    uchar i;
    uchar key = 0;
    
    P1 = 0xfb;    // 列扫描初值1111 1011
    
    for(i = 0; i <= 2; i++) {
        if(R1 == 0) {
            delay_ms(20);    // 消抖
            key = i + 1;  // 第一行按键值：1,2,3
        }
        if(R2 == 0) {
            delay_ms(20);
            key = i + 4;  // 第二行按键值：4,5,6
        }
        if(R3 == 0) {
            delay_ms(20);
            key = i + 7;  // 第三行按键值：7,8,9
        }
        
        delay_ms(100);
        temp = P1;
        temp = temp | 0xf0;
        temp = temp >> 1;
        temp = temp | 0xf0;
        P1 = temp;
    }
    
    return key;
}
