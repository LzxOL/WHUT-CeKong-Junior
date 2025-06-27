/*#include <reg51.h>
#include <stdio.h>

#define uchar unsigned char		  			
#define uint unsigned int
	
uchar dis[10]=//共阳极数码管字符0-9
{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar temp = 0;
uchar flag = 0;
uint i;

void delay(i)//延时函数
{	
	uchar t;
 	while (i--)	
		for(t=0;t<120;t++);	
}

void main(void)
 { 
	 TMOD = 0x20;//设置定时器T1为方式2
	 TH1 = 0xfd;//波特率9600
	 TL1 = 0xfd;
	 SCON = 0x50;//设置串口为方式1接收，REN=1
	 PCON = 0x00;//SMOD = 0
	 TR1 = 1;//启动T1
	 EA = 1;//总中断允许位
	 ES = 1;//打开接收中断
	 
	 P3=0xff;//解锁
	 P2=0x01;//开启WX0
	 P3=0x7f;//锁存
	 P2=0xff;
	 
	 while (1)
	{ 
		;		
	}
 }
 
void int1() interrupt 4
{
	if(RI)
	{
		temp = SBUF;
		RI = 0;	//清除接收中断标志位
		flag = 1;
		while(RI == 0)
		{
			P2 = dis[temp];
			if(flag==1)
			{
				//发送接收到字符
				SBUF=temp;
				while(!TI);
				TI=0;
				flag=0;
			}
			delay(10);
		}			
	}
}*/

#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
uint num;
/*uchar code DSY_CODE[]=//显示数组（共阴）
{
     0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f
};*/

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
     P2 = 0x00; //初始化
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


    while(1)
    {

    }
}

void Serial_INT() interrupt 4//中断显示
{
     if(RI)
    {
         RI = 0;
        num=SBUF+1;
        if(num>=0&&num<=9)
        {
            P2 = dis[num];//显示
            Delay(60);
            //P2 = 0x00;
        }
        else
            P2 = 0x00;//不显示
    }
}