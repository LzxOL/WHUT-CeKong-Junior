#include "stm32f10x.h"
#include "uart_pi.h"


extern int position_index;
extern int chuankoushuju;

void USART1_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

char USART1_ReceiveChar(void)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART1);
}

#define BUFFER_SIZE 256
char rx_buffer[BUFFER_SIZE];
uint16_t rx_index = 0;
uint8_t new_data = 0;

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        char rx_char = USART_ReceiveData(USART1);
        
        if (rx_char == 'A')
        {
//            rx_index = 0; 
        }
        else if (rx_char == 'B') 
        {
//            rx_buffer[rx_index] = '\0'; 
//            position_index = atoi(rx_buffer);
//            if (new_data == 0)
//            {
//                x_value = position_index;
//                new_data = 1;
//            }
//            else if (new_data == 1)
//            {
//                y_value = position_index;
//                new_data = 0;
//            }
//            rx_index = 0; 
        }
        else 
        {
//            if (rx_index < BUFFER_SIZE - 1)
//            {
//                rx_buffer[rx_index++] = rx_char; 
//            }
        }
    }
}

void USART1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // USART1 TX (PA.9)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART1 RX (PA.10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    //  NVIC  
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Ê¹ÄÜ USART1
    USART_Cmd(USART1, ENABLE);
}

void USART1_SendString(const char* str)
{
    while (*str)
    {
        USART1_SendChar(*str++);
    }
}
