#include "reg52.h"
#include "delay.h"

// ÑÓÊ±º¯Êý
void delay_ms(unsigned int t) {   
    unsigned int i=0, j=0;
	for(i=0; i<t; i++)
		for(j=0; j<120; j++);
}
