#ifndef _UART_H_
#define _UART_H_//写的源文件英文为小写这里也要用大写引用
void InitUART(void) ;

void weichoice(unsigned int i, unsigned int j) ;

void tempcount(unsigned int k) ;

void Delay(unsigned int i) ;


void main(void) ;


void SendByte(unsigned char dat) ;

void SendStr(unsigned char *s) ;
#endif
