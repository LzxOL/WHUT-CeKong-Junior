#include "stm32f10x.h" // Device header
#include "Delay.h"

// 初始化按键引脚 (GPIOB12-15，低电平触发)
void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 初始化控制引脚 (GPIOA1-5)
void Key_Init_A(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 设置初始状态
    GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5); // 初始关闭
    GPIO_SetBits(GPIOA, GPIO_Pin_2);                             // 初始打开
}

// 检测按键 1 是否按下 (低电平触发)
uint8_t Key_GetNum1(void)
{
    uint8_t KeyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) // 检测低电平
    {
        Delay_ms(20); // 消抖
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0); // 等待按键释放
            KeyNum = 1;
        }
    }
    return KeyNum;
}

// 检测按键 2 是否按下 (低电平触发)
uint8_t Key_GetNum2(void)
{
    uint8_t KeyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);
            KeyNum = 1;
        }
    }
    return KeyNum;
}

// 检测按键 3 是否按下 (低电平触发)
uint8_t Key_GetNum3(void)
{
    uint8_t KeyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
            KeyNum = 1;
        }
    }
    return KeyNum;
}

// 检测按键 4 是否按下 (低电平触发)
uint8_t Key_GetNum4(void)
{
    uint8_t KeyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);
            KeyNum = 1;
        }
    }
    return KeyNum;
}
