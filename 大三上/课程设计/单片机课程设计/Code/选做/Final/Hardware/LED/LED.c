#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

//LED_G(0): 灯亮 | LED_G(1):灯灭
void LED_Control(uint8_t n)                
{
    if(n)
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
    else
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}


void LED_OWN_Blinking(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	Delay_ms(1000);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	Delay_ms(1000);	
}

