#include <reg51.h>
#include <stdio.h>

#define uchar unsigned char		  			
#define uint unsigned int
	
uchar dis[10]=//共阳极数码管字符0-9
{0xc0,0xf9,0xa4,
0xb0,0x99,0x92,
0x82,0xf8,0x80,0x90};

uchar light;//AD光敏检测亮度//可扩展自动开灯if条件
uchar temp = 0;//串口通信中间变量
uchar flag = 0;//收发信息指示
uchar people = 0;//是否有人指示//可扩展自动亮灯

uint v_light;//亮度对应的电压

uchar Get_AD_Result();//AD检测函数

sbit LED = P0^0;	//板载LED控制口
sbit IR = P3^2;	//红外传感器数据口

//ADC0832接口
sbit CS = P3^5;
sbit CLK = P3^6;
sbit DIO = P3^7;

void delay(uint i)//延时函数
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
	 
	while(1)
	{
		//检测是否有人
		if(IR == 0)//检测到有人进入房间
			people = 1;//状态改变为有人
		
		while(people)//确认房间有人时
		{
			light=Get_AD_Result();
			v_light=(5000/256.0)*light;
			if(v_light > 4000)
				LED = 0;//亮灯
			else 
				LED = 1;//扩展关灯按键
				
		}
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
			P3=0xff;//解锁
			P2=0x01;//开启WX0
			P3=0x7f;//锁存
			P2=0xff;//清零
			P2 = dis[temp];
			if(flag==1)
			{
				//发送接收到字符
				SBUF=temp;
				while(!TI);
				TI=0;
				flag=0;
			}
			if(temp == 1)
				LED = 0;//LED亮
			
			delay(1000);
			P3=0xff;//解锁
			P2=0x00;//关闭位选
			P3=0x7f;//锁存
			delay(500);
			LED = 1;//LED灭	
			break;
		}			
	}
}

uchar Get_AD_Result()
{
    uchar i;
    uchar data1=0;
    CS=0;
  
    CLK=0;//第一个下降沿到来前，DI需置1，起始控制位，开始转换
	DIO=1;    
    CLK=1;
    
    CLK=0;//第二个下降沿到来前，设D=1/0，选择单端/差分（SGL/DIF）模式中的单端输入模式   
	DIO=1;
    CLK=1;           

    CLK=0;//第三个下降沿到来前，设D=0/1，选择CH0/CH1，这里选择单通道ch0     
	DIO=1;  
    CLK=1;
	DIO=0;   
   
    CLK=0;//第四个下降沿到来前,DI =1  
	DIO=1; 
    
    for(i=0;i<8;i++)//4-11，共8个下降沿 DO输出转换信号，读取数据（MSB-->LSB）
    {
        CLK=1;
        CLK=0;
       data1=(data1<<1)|(uchar)DIO;
    }
    CS=1;
    return data1;
}