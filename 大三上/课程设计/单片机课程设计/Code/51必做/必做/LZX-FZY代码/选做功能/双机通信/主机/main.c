#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
sbit K1 = P1^5;//按键口
uchar NumX = 5;//发送端数码管要显示的数

uchar dis[10]=//共阳极数码管字符0-9
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void Delay(uint x)//延时
{
     uchar i,j;
    for(i=x;i>0;i--)
         for(j=110;j>0;j--);
}
void main()
{
    SCON = 0x50;//串口方式1
    TMOD = 0x20;//定时器工作方式2
    PCON = 0x00;//波特率正常
    TH1  = 0xfd;//T1定时器装初值
    TL1  = 0xfd;
    TR1 = 1;//启动T1定时器 
    EA=1;//开总中断
    ES=1;//开启串口中断
	
	 P3=0xff;//解锁
	 P2=0x01;//开启WX0
	 P3=0x7f;//锁存
	 P2=0xff;
	
	P1=0xfb;//列扫描初值1111 1011，使p1.2为0,其余为1
   
    while(1)
    {
        if(K1==0);//按键按下
        {
            Delay(3);//消抖
             if (K1==0)//按键按下
             {
                     SBUF = NumX;//发送出去
                      P2 = dis[NumX];//显示
                      Delay(20);
                      P2 = 0x00;
                      while(!T1);//等待是否发送完毕
                       TI = 0;
                      while(!K1);//等待按键释放
              }
        }
    }
}