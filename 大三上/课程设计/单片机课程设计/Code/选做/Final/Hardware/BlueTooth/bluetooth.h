#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include <stdio.h>
void ClearPasswordBuffer(void);
void UART2_Init(void);

uint8_t Check_Bluetooth_Connection(void);

void HC06_State_Init(void);

void USART_SendByte(uint8_t Byte);

void USART_SendArray(uint8_t *Array, uint16_t Length);

void USART_SendString(char *String);

uint32_t USART_Pow(uint32_t X, uint32_t Y);

void USART_SendNumber(uint32_t Number, uint8_t Length);

uint8_t Serial_GetRxFlag(void);

uint8_t Serial_GetRxData(void);

int fputc(int ch, FILE *f);

void USART2_IRQHandler(void);
#endif
