#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "LED.h"
#include "OLED.h"
#define BUFFER_SIZE 20                  // 定义接收缓冲区大小



// 全局变量
volatile uint8_t Serial_RxData = 0;  // 存储接收到的数据
volatile uint8_t Serial_RxFlag = 0;  // 接收标志

// UART2 初始化函数
// 串口初始化函数
void UART2_Init(void) {
    // 启用GPIOA和USART2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 配置USART2 TX (PA2) 和 RX (PA3)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  // RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART2
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;  // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 8位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;  // 无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 不使用硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // 收发模式
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);  // 启用USART2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  // 使能接收中断

    // 配置中断
    NVIC_EnableIRQ(USART2_IRQn);  // 启用USART2中断
}

// 发送一个字节
void USART_SendByte(uint8_t Byte)
{
    USART_SendData(USART2, Byte);
    while (USART_GetFlagStatus(USART2, USART_IT_TXE) == RESET);  // 等待发送完成
}

// 发送字节数组
void USART_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        USART_SendByte(Array[i]);
    }
}

// 发送字符串
void USART_SendString(char *String)
{
    uint16_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        USART_SendByte(String[i]);
    }
}

// 计算 X 的 Y 次方
uint32_t USART_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

// 发送数字
void USART_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        USART_SendByte(Number / USART_Pow(10, Length - i - 1) % 10 + '0');
    }
}

// 重定向 printf 到 USART2
int fputc(int ch, FILE *f)
{
    USART_SendByte(ch);
    return ch;
}



// 获取接收标志
uint8_t Serial_GetRxFlag(void)
{
    return Serial_RxFlag;
}

// 获取接收到的数据
uint8_t Serial_GetRxData(void)
{
    Serial_RxFlag = 0;  // 清除接收标志
    return Serial_RxData;
}
