#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
    // 打开 TIM3 和 GPIOB 的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // 开启定时器3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 开启 GPIOB 时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // 复用推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;               // 选择 GPIOB1（对应 TIM3 通道2）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 设定输出速度为 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                   // 初始化 GPIOB1
    
    TIM_InternalClockConfig(TIM3);  // 设置 TIM3 时钟源为内部时钟源

    TIM_TimeBaseInitTypeDef TimBaseInitStructure;
    TimBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;          // 时钟分频
    TimBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;      // 向上计数模式
    TimBaseInitStructure.TIM_Period = 20000 - 1;                    // ARR = 20000，控制 PWM 的频率为 50Hz（50Hz = 20ms 周期）
    TimBaseInitStructure.TIM_Prescaler = 72 - 1;                     // PSC = 72-1，分频器设置为 72，频率为 1kHz
    TimBaseInitStructure.TIM_RepetitionCounter = 0;                  // 无重载
    TIM_TimeBaseInit(TIM3, &TimBaseInitStructure);                   // 初始化 TIM3 时基单元

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);                         // 初始化输出比较结构体
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 设置 PWM 模式为 PWM1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       // 输出比较极性为高电平有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // 输出状态使能
    TIM_OCInitStructure.TIM_Pulse = 0;                               // 初始占空比为 0（即 0% 占空比）
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);                        // 初始化 TIM3 通道 2（对应 GPIOB1）

    TIM_Cmd(TIM3, ENABLE);  // 启动 TIM3 定时器
}

void PWM_SetCompare4(uint16_t Compare)
{
    TIM_SetCompare4(TIM3, Compare);  // 设置输出比较寄存器值（调整 PWM 占空比）
}
