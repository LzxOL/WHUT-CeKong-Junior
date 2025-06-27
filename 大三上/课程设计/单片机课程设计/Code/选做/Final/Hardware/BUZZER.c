#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#define Buzzer_Pin GPIO_Pin_1 
void Buzzer_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = Buzzer_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, Buzzer_Pin);
}

void Buzzer_ON()
{
	GPIO_ResetBits(GPIOA, Buzzer_Pin);
}

void Buzzer_OFF()
{
	GPIO_SetBits(GPIOA, Buzzer_Pin);
}

void Buzzer1()	//·äÃùÆ÷ÏìÒ»Éù
{
	Buzzer_ON();
	Delay_ms(500);
	Buzzer_OFF();
}

void Buzzer2()	//·äÃùÆ÷ÏìÁ½Éù
{
	Buzzer_ON();
	Delay_ms(100);
	Buzzer_OFF();
	Delay_ms(100);
	Buzzer_ON();
	Delay_ms(100);
	Buzzer_OFF();
}

void Buzzer_Alarm()	//·äÃùÆ÷·¢³ö¾¯±¨
{
	Buzzer_ON();
	Delay_ms(50);
	Buzzer_OFF();
	Delay_ms(50);
	Buzzer_ON();
	Delay_ms(50);
	Buzzer_OFF();
	Delay_ms(50);
	Buzzer_ON();
	Delay_ms(50);
	Buzzer_OFF();
	Delay_ms(50);
	Buzzer_ON();
	Delay_ms(50);
	Buzzer_OFF();
}
void alarm(void){
	
	Buzzer_ON();
	Delay_ms(100);
	Buzzer_OFF();
	Delay_ms(100);

	Buzzer_ON();
	Delay_ms(100);
	Buzzer_OFF();
}