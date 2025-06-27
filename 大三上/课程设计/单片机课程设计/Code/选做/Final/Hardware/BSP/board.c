#include "board.h"

/**********************************************************
*** ZDT_X42_V2.0步进闭环控制例程
*** 编写作者：ZHANGDATOU
*** 技术支持：张大头闭环伺服
*** 淘宝店铺：https://zhangdatou.taobao.com
*** CSDN博客：http s://blog.csdn.net/zhangdatou666
*** qq交流群：262438510
**********************************************************/

/**
	*	@brief		外设时钟初始化
	*	@param		无
	*	@retval		无
	*/
extern uint8_t board_move_i;                      // 接收到的数据
extern uint8_t board_move_mode,rfid_mode,blue_tooth_mode;                      // 接收到的数据

void clock_init(void)
{
	// 使能GPIOA端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

/**
	*	@brief		gpio引脚初始化
	*	@param		无
	*	@retval		无
	*/
void gpio_init(void)
{
	// 初始化PA5(En) PA6(Stp) PA7(Dir)端口为推挽输出模式
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 初始化PA5(En) PA6(Stp) PA7(Dir)端口为低电平输出
	GPIO_ResetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
}

/**
	*	@brief		板载初始化
	*	@param		无
	*	@retval		无
	*/
void board_init(void)
{
	clock_init();
	gpio_init();
}

extern unsigned char board_turn_flag_cw;  // 顺时针标志位
extern unsigned char board_turn_flag_ccw; // 顺时针转动函数

//开门
void rotate90_degrees(void) {
    static uint16_t pulse_count = 0;

    // 如果已经顺时针转过90度，就不再顺时针转
    if (board_turn_flag_cw == 1) {
        return;  // 已经执行过顺时针转动，直接返回
    }

    // 设置电机方向为正转（顺时针）
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);  // 方向信号输出低电平，电机正转

    // 输出一个脉冲
    GPIO_SetBits(GPIOB, GPIO_Pin_6);    // 高电平
    delay_cnt(100);                     // 高电平持续时间
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // 低电平
    delay_cnt(100);                     // 低电平持续时间

    // 计数加1
    pulse_count++;

    // 每10个脉冲更新一次OLED显示
    if(pulse_count % 10 == 0) {
        OLED_ShowNum(3, 10, pulse_count, 3);
    }

    // 检查是否完成800个脉冲(90度)
    if(pulse_count >= 800) {
        pulse_count = 0;                 // 重置计数器
        Delay_ms(1000);                  // 延时1秒
        OLED_ShowString(3, 1, "Done");
        OLED_Clear();
        board_move_mode = 2;             // 切换到完成状态
        rfid_mode = 2;
        blue_tooth_mode = 2;

        // 设置顺时针标志位为1，表示顺时针转动已完成
        board_turn_flag_cw = 1;

    }
}

// 关门转动函数
void rotate90_degrees_reverse(void) {
    static uint16_t pulse_count = 0;

    // 如果已经逆时针转过90度，就不再逆时针转
    if (board_turn_flag_ccw == 1) {
        return;  // 已经执行过逆时针转动，直接返回
    }

    // 设置电机方向为反转（逆时针）
    GPIO_SetBits(GPIOB, GPIO_Pin_7);  // 方向信号输出高电平，电机反转

    // 输出一个脉冲
    GPIO_SetBits(GPIOB, GPIO_Pin_6);    // 高电平
    delay_cnt(100);                     // 高电平持续时间
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);  // 低电平
    delay_cnt(100);                     // 低电平持续时间

    // 计数加1
    pulse_count++;

    // 每10个脉冲更新一次OLED显示
    if(pulse_count % 10 == 0) {
        OLED_ShowNum(3, 10, pulse_count, 3);
    }

    // 检查是否完成800个脉冲(90度)
    if(pulse_count >= 800) {
        pulse_count = 0;                 // 重置计数器
        Delay_ms(1000);                  // 延时1秒
        OLED_ShowString(3, 1, "Done");
        OLED_Clear();
        board_move_mode = 2;             // 切换到完成状态
        rfid_mode = 2;
        blue_tooth_mode = 2;

        // 设置逆时针标志位为1，表示逆时针转动已完成
        board_turn_flag_ccw = 1;
    }
}
uint8_t ii =0;

void close_door(void){
	GPIO_SetBits(GPIOB, GPIO_Pin_7);  	// 方向信号输出低电平，电机正转
	for(ii=0; ii < 800; ii++)             	// 输出3200个脉冲信号
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_6);    // 输出高电平 
		delay_cnt(1000+ii*20);           					// 高电平持续时间
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);	// 输出低电平 
		delay_cnt(1000+ii*20);           					// 低电平持续时间
		if(ii % 10 == 0) {
        OLED_ShowNum(3, 10, ii, 3);
    }
	}
	if(ii>=799){
        Delay_ms(1000);                  // 延时1秒
        OLED_ShowString(3, 1, "Done");
        OLED_Clear();
        board_move_mode = 2;             // 切换到完成状态
        rfid_mode = 2;
        blue_tooth_mode = 2;

        // 设置顺时针标志位为1，表示顺时针转动已完成
        board_turn_flag_cw = 1;
	}
}