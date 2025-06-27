#ifndef __LCD1602_H_
#define __LCD1602_H_

#include "main.h"

#define _LCD_COLS         16
#define _LCD_ROWS         2

/*****  GPIOB3  H:data;L:command  ******/
#define  LCD1602_RS_DATA	   	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)		
#define  LCD1602_RS_CMD			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET)
/*****  GPIOB4  H:read;L:write  ******/
#define  LCD1602_RW_READ        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)
#define  LCD1602_RW_WRITE       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)
/*****  GPIOB5    ******/
#define  LCD1602_E_HIGH         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)
#define  LCD1602_E_LOW          HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)

#define  LCD1602_DATA(x)				GPIOB->ODR=(GPIOB->ODR & 0x00ff) | (x<<8)

/* ָ� */
#define LCD_CLEARDISPLAY        0x01	//����ָ��
#define LCD_RETURNHOME          0x02	//��ַ������ AC=0;����ʱ��ַΪ 0x80�� ����ԭ�㣬���� DDRAM �ж����ݲ���
#define LCD_ENTRYMODESET        0x04	//����ģʽ����
#define LCD_DISPLAYCONTROL      0x08	//ֻ����ʾ
#define LCD_CURSORSHIFT         0x10	
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

/* ��ʾ���ģʽ�ı�־ */
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00
/* ���ؿ��Ʊ�־ */
#define LCD_DISPLAYON           0x04
#define LCD_CURSORON            0x02
#define LCD_BLINKON             0x01

#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

typedef struct {
uint8_t DisplayControl;
uint8_t DisplayFunction;
uint8_t DisplayMode;
uint8_t currentX;
uint8_t currentY;
} LCD_Options_t;

void LCD_Init(void);	//��Ļ��ʼ��	
void LCD_DisplayOn(void);	
void LCD_DisplayOff(void);
void LCD_Clear(void);	//����
void LCD_Puts(uint8_t x, uint8_t y, char* str);		//��ʾ�ַ���
void LCD_BlinkOn(void);
void LCD_BlinkOff(void);
void LCD_CursorOn(void);
void LCD_CursorOff(void);
void LCD_ScrollLeft(void);
void LCD_ScrollRight(void);
void LCD_CreateChar(uint8_t location, uint8_t* data);
void LCD_PutCustom(uint8_t x, uint8_t y, uint8_t location);
void LCD_Put(uint8_t Data);
void LCD_ShowNum(unsigned char Line,unsigned char Columu,unsigned int Num,unsigned int Length);
void LCD_ShowNum1(uint8_t x, uint8_t y,uint8_t num);
int LCD_Pow(unsigned char x,unsigned char y);
#endif
