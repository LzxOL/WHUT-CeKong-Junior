#ifndef __UART_PI_H
#define __UART_PI_H

void USART1_SendChar(char c);
char USART1_ReceiveChar(void);
void USART1_IRQHandler(void);
void USART1_Init(void);
void USART1_SendString(const char* str);

#endif
