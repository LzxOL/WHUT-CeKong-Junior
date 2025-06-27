#include<reg52.h>
#include<stdio.h>
#define uchar unsigned char	
#define uint  unsigned int 
	
uchar Get_AD_Result1();  

uchar dis[10]=//共阳极数码管字符0-9
{0xc0,0xf9,0xa4,
0xb0,0x99,0x92,
0x82,0xf8,0x80,0x90};

unsigned char date[4];//ADC0832
sbit CS   = P3^5;
sbit CLK  = P3^6;
sbit DIDO = P3^7;

void delay(unsigned int t)
{
	unsigned int i=0, j=0;
	for(i=0; i<t; i++)
		for(j=0; j<120; j++);
}					
					
void DigDisplay()
{
	int i;
	for(i=0;i<4;i++)
	{
		switch(i)
		{
			case(0):
			P3=0xff;//解锁
			P2=0x01;//开启WX0
			P3=0x7f;//锁存
			P2=date[i]&0x7f;
			break;
			
			case(1):
			P3=0xff;//解锁
			P2=0x02;//开启WX1
			P3=0x7f;//锁存
			P2=date[i];
			break;
			
			case(2):
			P3=0xff;//解锁
			P2=0x04;//开启WX2
			P3=0x7f;//锁存
			P2=date[i];
			break;

			case(3):
			P3=0xff;//解锁
			P2=0x08;//开启WX3
			P3=0x7f;//锁存
			P2=date[i];
			break;
		}
		delay(5);
	}
}

void main(void)
{
	uchar AD1;
	uint vt1;   
	uint while_times = 0;
	
	delay(300);
	ET0  = 1;
    TR0  = 1;
    ET1  = 1;
    TR1  = 1;
    EA   = 1;     
    CLK  = 0;      
    CS   = 1; 
	 
	while(1)
	{	
	   AD1 = Get_AD_Result1();
	   vt1=	(5000/256.0)*AD1;
		date[0] =dis[vt1/1000];
		date[1] =dis[vt1/100%10]; 
		date[2] =dis[vt1%100/10];
		date[3] =dis[vt1%10];
		DigDisplay(); 	
	}
}

uchar Get_AD_Result1()     
{
    uchar i, dat;
    CS = 1;              
    CLK =1;                  
    CS = 0;
    DIDO = 1; CLK = 0;  CLK = 1; 
    DIDO = 1; CLK = 0;  CLK = 1;  
    DIDO = 0; CLK = 0;  CLK = 1; 
    DIDO = 1;                               
    for(i = 0; i < 8; i++)                  
    {
        CLK = 0;                           
        if(DIDO) dat |= 0x01;              
        CLK = 1;                       
        dat <<= 1;                          
    }
    return (dat);
    //CS = 1;                                  
}