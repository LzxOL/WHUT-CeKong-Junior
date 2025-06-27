#include "timer.h"




//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
			 
}
////定时器3中断服务程序
//void TIM3_IRQHandler(void)   //TIM3中断
//{

//}


