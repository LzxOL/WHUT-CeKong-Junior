#include "stm32f10x.h"                  // 设备头文件
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Menu.h"
#include "RC522.h"
#include "Flash.h"
#include "timer.h"
#include "bluetooth.h"
#include "Servo.h"
#include "board.h"
#include <stdbool.h>        // 布尔类型支持

uint8_t inPasswordMode = 0; // 是否进入密码模式
uint8_t inPasswordMode_PD_Mode = 0;

uint8_t passwordPos = 0;  // 密码输入位置
uint8_t password_client[4] = {0};
uint8_t correct_password[4] = {1, 1, 1, 1};  // 正确密码，1, 2, 3, 4
uint8_t new_password[4]={0};
#define PASSWORD_LENGTH 4

uint8_t cardnumber, KeyNum, tempcard, select = 0, flag_scan = 0, flag_addcard = 0, flag_deletecard = 0, flag_clear = 0;
uint8_t pd_enter_flag = 0,pd_change_flag =0;
uint8_t key_last = 0, key_err = 0, stop_flag = 0;
/* 串口相关变量 */
uint8_t count = 0, clear_flag, reset_flag = 0;
/* 蓝牙 */
uint8_t bluetooth_password[4] = {0};  // 用于存储蓝牙的输入密码
uint8_t bt_password_pos = 0;          // 蓝牙密码输入位置
uint8_t bluetooth_connected = 0;       // 蓝牙连接状态
uint8_t RxData;                       // 接收到的数据
uint8_t bluetooth_open_flag = 0;       // 蓝牙连接状态

#define BUFFER_SIZE 20                  // 定义接收缓冲区大小
#define PASSWORD_END_CHAR ' '           // 密码结束符
#define MAX_ERROR_COUNT 3  // 最大错误次数
#define MAX_CLOSE_COUNT 30  // 最大错误次数

static uint8_t error_count = 0;  // 错误次数计数器
uint8_t error_count_last_time = 0;  // 错误次数计数器

char receivedPassword[BUFFER_SIZE];       // 用于存储接收到的密码
char last_receivedPassword[BUFFER_SIZE];  // 用于存储上一次接收到的密码

volatile uint8_t passwordIndex = 0;       // 密码索引
volatile bool passwordReceived = false;   // 标志，指示是否接收到完整密码


uint8_t board_move_flag = 0;;                 
uint8_t board_move_i = 0;                     
uint8_t board_move_mode = 0;;                 

uint8_t rfid_mode = 0;                    
uint8_t rfid_display_flag = 0;                      
uint8_t password_display_flag = 0;                    
uint8_t password_change_flag = 0,password_change_mode =0;                    

uint8_t blue_tooth_mode = 0;;                    
uint8_t isCorrect_change_pd ;
uint8_t first_change_key = 0 ;
uint8_t isCorrect_flag = 0;
// 清空接收密码缓冲区
void ClearPasswordBuffer(void) {
    memset(receivedPassword, 0, sizeof(receivedPassword));
    passwordIndex = 0;
}
// 初始化USART2的中断优先级和使能
void Init_USART2_NVIC(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    // 配置USART2中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 主优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
extern uint8_t UID[4], Temp[4];
extern uint8_t UI0[4]; // 卡片0 ID数组
extern uint8_t UI1[4]; // 卡片1 ID数组
extern uint8_t UI2[4]; // 卡片2 ID数组
extern uint8_t UI3[4]; // 卡片3 ID数组

void RFID_Check(void);
void Read_Card(void);
static uint8_t isFirstEnter = 1;
// 定义标志位，初始值为 0，表示未执行过相应的转动
unsigned char board_turn_flag_cw = 0;  // 顺时针标志位
unsigned char board_turn_flag_ccw = 0; // 逆时针标志位
uint8_t pd_enter_after_change = 0;
void reset_flag_all(){
	inPasswordMode = 0;
	OLED_Clear();
	displayMainMenu();
	isFirstEnter = 1;
	rfid_display_flag = 0;
					flag_scan = 0;
				flag_addcard = 0;
				flag_deletecard = 0;
	//标志位情空
	board_move_mode = 0;             // 切换到完成状态
	rfid_mode = 0;
	blue_tooth_mode = 0;
	
	// 设置逆时针标志位为1，表示逆时针转动已完成
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

}



// 系统初始化函数
void system_init() {
    uint8_t stored_password[PASSWORD_LENGTH];
    LoadPasswordFromFlash(stored_password);  // 从Flash读取密码

    // 检查密码是否有效
    uint8_t isPasswordValid = 1;
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (stored_password[i] != correct_password[i]) {
            isPasswordValid = 0;  // 密码不匹配
            break;
        }
    }
isPasswordValid = 1;
    if (!isPasswordValid) {
        // 如果密码无效或没有密码，写入初始密码
        SavePasswordToFlash(correct_password);
        OLED_Clear();
        OLED_ShowString(1, 1, "Password set!");  // 显示密码已设置
        Delay_ms(1000);
    } else {
        // 如果密码有效
			
		// 初始化完成后，显示"System init ok"
		OLED_Clear();
		OLED_ShowString(1, 1, "System init ok");  // 显示初始化成功
		Delay_ms(500);
		OLED_Clear();
    }



}
int main(void)
{
    OLED_Init();       // OLED初始化
    Key_Init();        // 按键初始化
    RFID_Init();       // RFID初始化
    Read_Card();       // 读取卡片信息
    Timer_Init();      // 定时器初始化
	Buzzer_Init();
	board_init();
	UART2_Init();       // 初始化USART2，波特率9600
    Init_USART2_NVIC();
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能USART2接收中断
	
	system_init();
	displayMainMenu(); // 显示主菜单
//SavePasswordToFlash(correct_password);
	// 上电延时2秒等待ZDT_X42_V2闭环初始化完毕
    while (1)
    {
        KeyNum = Key_Scan();  // 扫描按键
        key_err = KeyNum - key_last;
        key_last = KeyNum;
        if (error_count>=MAX_ERROR_COUNT) {// 当错误次数超过五次时
            // 根据按键执行不同操作
			if (isFirstEnter) {
				
				OLED_Clear();  // 清屏
				isFirstEnter = 0;
				inPasswordMode = 0;
				rfid_display_flag = 0;
				flag_scan = 0;
				flag_addcard = 0;
				flag_deletecard = 0;
				//标志位情空
				board_move_mode = 0;      // 切换到完成状态
				rfid_mode = 0;
				blue_tooth_mode = 0;
				
				// 设置逆时针标志位为1，表示逆时针转动已完成
				TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			}			
			OLED_ShowString(1, 1, "error times");
			OLED_ShowString(2, 1, "try again ");
			error_count_last_time = MAX_CLOSE_COUNT - count;
			OLED_ShowNum(2, 11,error_count_last_time,2);
			OLED_ShowString(2,13, "s ");
			
			if(count >=MAX_CLOSE_COUNT) OLED_Clear();
			
        } 
		else {
            if (!inPasswordMode) {
                switch (KeyNum) {
                    case 1:
                        OLED_Clear();
                        inPasswordMode = 1; // 按键密码进入
						TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); //关闭时钟
                        break;
                    case 2:
                        OLED_Clear();
                        OLED_ShowString(1, 1, "RFID");
                        inPasswordMode = 2;// RFID进入
						TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

                        break;
                    case 3:
                        OLED_Clear();
                        inPasswordMode = 3;// 蓝牙进入
						TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
                        break;
                }
            }
            else if (inPasswordMode == 1) {  // 输入密码
				
				switch(inPasswordMode_PD_Mode){
					case 0:
						if(!password_display_flag){
							password_display_flag = 1;
							OLED_ShowString(1, 1, "Keys Mode");
							OLED_ShowString(2, 1, "1.PD Enter");
							OLED_ShowString(3, 1, "2.Change PD");				
						}
						// 按键处理
						if (KeyNum == 1) {// 进入按键密码的模式
							OLED_Clear();
							OLED_ShowString(1, 1, "Pls enter pd:");
							inPasswordMode_PD_Mode = 1;
						}
						if (KeyNum == 2) {// 进入修改密码的模式
							OLED_Clear();
							OLED_ShowString(1, 1, "Pls enter cur pd:");
							inPasswordMode_PD_Mode = 2;
						}
						
						if(KeyNum==4 && (pd_enter_flag != 1 && pd_change_flag !=1))	
						{
							OLED_Clear();
			
							inPasswordMode = 0;
							OLED_Clear();
							displayMainMenu();
							isFirstEnter = 1;
							password_display_flag = 0;
						}
					break;
					case 1:
						if (KeyNum >= 1 && KeyNum <= 4 && passwordPos < PASSWORD_LENGTH && !board_move_mode) {

							password_client[passwordPos] = KeyNum;
							passwordPos++;
							OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);

							if (passwordPos >= PASSWORD_LENGTH) {
								uint8_t isCorrect = 1;  // 检查密码是否正确
								uint8_t flash_password[PASSWORD_LENGTH];	
								
								// 从 Flash 中加载密码到 correct_password
								LoadPasswordFromFlash(flash_password);
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
									correct_password[i] = flash_password[i];  // 更新 correct_password
								}
								
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
										if (password_client[i] != correct_password[i]) {
											isCorrect = 0;  // 密码错误
											break;
										}
									}
									Delay_ms(1000);
									OLED_Clear();
									OLED_ShowString(1, 1, "Checking...");
									Delay_ms(1000);
									OLED_Clear();
									if (isCorrect) {
										OLED_ShowString(1, 1, "Yes");
										Delay_ms(500);
										// 密码正确，执行相关操作
										board_move_mode = 1; // 进入逻辑判断阶段
										
									} else {
																				error_count ++; //记录错误次数

										OLED_ShowString(1, 1, "No");
										uint8_t times_left = MAX_ERROR_COUNT-error_count;
										OLED_ShowNum(2,1,times_left,1);
										OLED_ShowString(2, 2, " times left");
										Delay_ms(500);
										// 密码错误，执行相应提示
										board_move_mode = 0;
										
										
										inPasswordMode = 0;
										passwordPos = 0;
										inPasswordMode_PD_Mode = 0;
										OLED_Clear();
										displayMainMenu();
										TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
										password_display_flag = 0;
									}
							}
							
						 }
						
						else if(board_move_mode == 1) {
							rotate90_degrees();// 开门
						}
						else if(board_move_mode == 2) {
							// 重置状态
							inPasswordMode = 0;
							passwordPos = 0;
							inPasswordMode_PD_Mode = 0;
							board_move_mode = 0;
							OLED_Clear();
							displayMainMenu();
							TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
						}
					break;
					case 2:
						if (!password_display_flag) {
							password_display_flag = 1;
							OLED_ShowString(1, 1, "Change Password");
							OLED_ShowString(2, 1, "Pls enter cur pd:");  // 提示用户输入当前密码
						}

						// 输入当前密码
						if (KeyNum >= 1 && KeyNum <= 4 && passwordPos < PASSWORD_LENGTH && !isCorrect_flag) {
							password_client[passwordPos] = KeyNum;  // 存储用户输入的密码位
							passwordPos++;
							OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // 显示输入的密码位
						
							// 如果已经输入了完整的当前密码，进行密码校验
							if (passwordPos >= PASSWORD_LENGTH && !password_change_flag) {
								isCorrect_change_pd = 1;  // 假设密码正确，进行比对
								uint8_t stored_password[PASSWORD_LENGTH];
								LoadPasswordFromFlash(stored_password);  // 从Flash读取密码
								
								uint8_t flash_password[PASSWORD_LENGTH];	
								
								// 从 Flash 中加载密码到 correct_password
								LoadPasswordFromFlash(flash_password);
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
									correct_password[i] = flash_password[i];  // 更新 correct_password
								}
								
								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
									if (password_client[i] != correct_password[i]) {
										isCorrect_change_pd = 0;  // 密码错误
										break;
									}
								}

								Delay_ms(1000);  // 显示延时，给用户一些缓冲时间
								OLED_Clear();
								OLED_ShowString(1, 1, "Checking...");  // 提示正在验证密码
								Delay_ms(1000);
								OLED_Clear();

								if (isCorrect_change_pd) {
									// 当前密码正确，允许用户输入新密码
									passwordPos = 0;  // 清空输入位置，准备输入新密码
									password_change_flag = 1;
									isCorrect_flag = 1;
									OLED_Clear();
									OLED_ShowString(1, 1, "Pls enter new pd");  // 提示输入新密码
									first_change_key = 1;
								} else {
									// 当前密码错误，重新开始输入
									OLED_ShowString(1, 1, "Incorrect password");
									Delay_ms(500);
									passwordPos = 0;  // 清空输入密码
									OLED_Clear();
									OLED_ShowString(1, 1, "Pls enter pd:");  // 提示重新输入当前密码
								}
							}
						}

						// 如果输入正确的当前密码，继续输入新密码
						if (isCorrect_change_pd && passwordPos < PASSWORD_LENGTH && password_change_flag == 1) {
							if (first_change_key == 1) {
								first_change_key = 0;
								KeyNum = 0;
								passwordPos = 0;
							}
							if (KeyNum >= 1 && KeyNum <= 4) {  // 确保按键未被处理
								new_password[passwordPos] = KeyNum;  // 存储新密码的当前位
								passwordPos++;
								OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // 显示新密码位
							}
						}

						// 当新密码输入完成时
						if (passwordPos >= PASSWORD_LENGTH && password_change_flag == 1) {
							// 完成新密码的输入，进行确认
							password_change_flag = 2;  // 标记新密码输入完成，准备进行确认
						}

						// 当新密码输入完成时
						if (isCorrect_change_pd && passwordPos >= PASSWORD_LENGTH && password_change_flag == 2) {
							// 完成新密码的输入，进行确认
							OLED_Clear();
							OLED_ShowString(1, 1, "New password set");  // 显示设置成功的消息

							// 保存新密码
							for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
								correct_password[i] = new_password[i];  // 更新密码
							}

							// 将新密码写入Flash
							SavePasswordToFlash(correct_password);

							Delay_ms(1000);  // 显示修改成功，延时1秒
							OLED_Clear();
							displayMainMenu();  // 返回主菜单
							password_display_flag = 0;  // 清除密码显示标志
							passwordPos = 0;  // 清空密码输入位置
							inPasswordMode_PD_Mode = 0;  // 退出密码修改模式
							inPasswordMode = 0;
							password_change_flag = 0;
							isCorrect_change_pd = 0;

						}
						
//						if (!password_display_flag) {
//							password_display_flag = 1;
//							OLED_ShowString(1, 1, "Change Password");
//							OLED_ShowString(2, 1, "Pls enter cur pd:");  // 提示用户输入当前密码
//						}

//						// 输入当前密码
//						if (KeyNum >= 1 && KeyNum <= 4 && passwordPos < PASSWORD_LENGTH && !isCorrect_flag) {
//							password_client[passwordPos] = KeyNum;  // 存储用户输入的密码位
//							passwordPos++;
//							OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // 显示输入的密码位
//							// 如果已经输入了完整的当前密码，进行密码校验
//							if (passwordPos >= PASSWORD_LENGTH && !password_change_flag) {
//								isCorrect_change_pd = 1;  // 假设密码正确，进行比对
//								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
//									if (password_client[i] != correct_password[i]) {
//										isCorrect_change_pd = 0;  // 密码错误
//										break;
//									}
//								}

//								Delay_ms(1000);  // 显示延时，给用户一些缓冲时间
//								OLED_Clear();
//								OLED_ShowString(1, 1, "Checking...");  // 提示正在验证密码
//								Delay_ms(1000);
//								OLED_Clear();

//								if (isCorrect_change_pd) {
//									// 当前密码正确，允许用户输入新密码
//									passwordPos = 0;  // 清空输入位置，准备输入新密码
//									password_change_flag = 1;
//									isCorrect_flag = 1;
//									OLED_Clear();
//									OLED_ShowString(1, 1, "Pls enter new pd");  // 提示输入新密码
//									first_change_key = 1;
//								} else {
//									// 当前密码错误，重新开始输入
//									OLED_ShowString(1, 1, "Incorrect password");
//									Delay_ms(500);
//									passwordPos = 0;  // 清空输入密码
//									OLED_Clear();
//									OLED_ShowString(1, 1, "Pls enter pd:");  // 提示重新输入当前密码
//								}
//							}
//						}

//							// 如果输入正确的当前密码，继续输入新密码
//							if (isCorrect_change_pd && passwordPos < PASSWORD_LENGTH && password_change_flag == 1) {
//								if(first_change_key == 1){
//									first_change_key = 0;
//									KeyNum = 0;
//									passwordPos = 0;
//								}
//								if (KeyNum >= 1 && KeyNum <= 4) {  // 确保按键未被处理
//									new_password[passwordPos] = KeyNum;  // 存储新密码的当前位
//									passwordPos++;
//									OLED_ShowNum(2, passwordPos + 1, KeyNum, 1);  // 显示新密码位
//								}
//							}

//							// 当新密码输入完成时
//							if (passwordPos >= PASSWORD_LENGTH && password_change_flag == 1) {
//								// 完成新密码的输入，进行确认
//								password_change_flag = 2;  // 标记新密码输入完成，准备进行确认
//							}
//														// 当新密码输入完成时
//							if (isCorrect_change_pd && passwordPos >= PASSWORD_LENGTH && password_change_flag == 2) {
//								// 完成新密码的输入，进行确认
//								OLED_Clear();
//								OLED_ShowString(1, 1, "New password set");  // 显示设置成功的消息
//								// 保存新密码
//								for (uint8_t i = 0; i < PASSWORD_LENGTH; i++) {
//									correct_password[i] = new_password[i];  // 更新密码
//								}

//								Delay_ms(1000);  // 显示修改成功，延时1秒
//								OLED_Clear();
//								displayMainMenu();  // 返回主菜单
//								password_display_flag = 0;  // 清除密码显示标志
//								passwordPos = 0;  // 清空密码输入位置
//								inPasswordMode_PD_Mode = 0;  // 退出密码修改模式
//								inPasswordMode = 0;
//								password_change_flag = 0;
//							}
//						
//						break;
					default:
						break;
				
				}
            }
            else if (inPasswordMode == 2) {  // RFID模式
				
				if(!rfid_display_flag){
					OLED_ShowString(1, 1, "RFID Mode");
					OLED_ShowString(2, 1, "1.Scan");
					OLED_ShowString(3, 1, "2.Add card");
					OLED_ShowString(4, 1, "3.delete");
					rfid_display_flag = 1;
				}

				
                // 按键处理
                if (KeyNum == 1) {// 进入刷卡模式
                    flag_scan = 1;
                    flag_addcard = 0;
                    flag_deletecard = 0;

                }
                if (KeyNum == 2) {// 进入写卡模式
                    flag_scan = 0;
                    flag_addcard = 1;
                    flag_deletecard = 0;

                }
                if (KeyNum == 3) {// 进入删卡模式
                    flag_scan = 0;
                    flag_addcard = 0;
                    flag_deletecard = 1;

                }
				if(KeyNum==4)	//按下4键退出删卡模式，进入寻卡模式
				{
					OLED_Clear();
					
					flag_scan=0;
					flag_addcard=0;
					flag_deletecard=0;
					
					inPasswordMode = 0;
					OLED_Clear();
					displayMainMenu();
					isFirstEnter = 1;
					rfid_display_flag = 0;
				}
					
                if (flag_scan == 1) {
                    RFID_Check();
					if(rfid_mode == 1){
//						Buzzer2();				// 发出开锁声音
						rotate90_degrees();
//						WaitCardOff();			// 等待卡片移开
//						
//						inPasswordMode = 0;
//						OLED_Clear();
//						displayMainMenu();
					}
					else if(rfid_mode == 2){
						Delay_ms(1000);
						reset_flag_all();
					}

					if(KeyNum==4)	//按下4键退出删卡模式，进入寻卡模式
					{
						OLED_Clear();
						flag_scan=0;
						flag_addcard=0;
						flag_deletecard=0;
						inPasswordMode = 0;
						OLED_Clear();
						displayMainMenu();
						isFirstEnter = 1;
						rfid_display_flag = 0;
						
					}
					
                }

                while (flag_addcard == 1) {  // 添加卡片
					if (isFirstEnter) {
                        OLED_Clear();  // 清屏
                        isFirstEnter = 0;
                    }
					
					OLED_ShowString(1, 1, "   Add Card:    ");

                    KeyNum = Key_Scan();
                    if (KeyNum == 1) {  // 返回到扫描模式
                        flag_scan = 1;
                        flag_addcard = 0;
                        flag_deletecard = 0;
                    }
                    if (KeyNum == 3) {  // 返回到删除卡片模式
                        flag_scan = 0;
                        flag_addcard = 0;
                        flag_deletecard = 1;
                    }
					if(KeyNum==4)	//按下4键退出删卡模式，进入寻卡模式
					{
						
						flag_scan=0;
						flag_addcard=0;
						flag_deletecard=0;
						
						OLED_Clear();

						inPasswordMode = 2;
						isFirstEnter = 1;
						rfid_display_flag = 0;
					}


                    if (PcdRequest(REQ_ALL, Temp) == MI_OK) {
                        if (PcdAnticoll(UID) == MI_OK) {
                            // 检查卡片ID是否为已有卡片
                            if (UI0[0] == 0xFF && UI0[1] == 0xFF && UI0[2] == 0xFF && UI0[3] == 0xFF)
                                tempcard = 0;
                            else if (UI1[0] == 0xFF && UI1[1] == 0xFF && UI1[2] == 0xFF && UI1[3] == 0xFF)
                                tempcard = 1;
                            else if (UI2[0] == 0xFF && UI2[1] == 0xFF && UI2[2] == 0xFF && UI2[3] == 0xFF)
                                tempcard = 2;
                            else if (UI3[0] == 0xFF && UI3[1] == 0xFF && UI3[2] == 0xFF && UI3[3] == 0xFF)
                                tempcard = 3;
                            else
                                tempcard = 4;

                            // 判断卡片是否已存在
                            if (UID[0] == UI0[0] && UID[1] == UI0[1] && UID[2] == UI0[2] && UID[3] == UI0[3])
                                tempcard = 5;
                            if (UID[0] == UI1[0] && UID[1] == UI1[1] && UID[2] == UI1[2] && UID[3] == UI1[3])
                                tempcard = 5;
                            if (UID[0] == UI2[0] && UID[1] == UI2[1] && UID[2] == UI2[2] && UID[3] == UI2[3])
                                tempcard = 5;
                            if (UID[0] == UI3[0] && UID[1] == UI3[1] && UID[2] == UI3[2] && UID[3] == UI3[3])
                                tempcard = 5;

                            switch (tempcard) {
                                case 0:
                                    UI0[0] = UID[0];
                                    UI0[1] = UID[1];
                                    UI0[2] = UID[2];
                                    UI0[3] = UID[3];
                                    FLASH_W(FLASH_ADDR1, UI0[0], UI0[1], UI0[2], UI0[3]);  // 写入闪存
                                    OLED_ShowString(1, 1, "  Add Card 1 OK ");
                                    Buzzer1();
                                    WaitCardOff();  // 等待卡片移开
                                    OLED_Clear();
                                    break;
                                case 1:
                                    UI1[0] = UID[0];
                                    UI1[1] = UID[1];
                                    UI1[2] = UID[2];
                                    UI1[3] = UID[3];
                                    FLASH_W(FLASH_ADDR2, UI1[0], UI1[1], UI1[2], UI1[3]);
                                    OLED_ShowString(1, 1, "  Add Card 2 OK ");
                                    Buzzer1();
                                    WaitCardOff();
                                    OLED_Clear();
                                    break;
                                case 2:
                                    UI2[0] = UID[0];
                                    UI2[1] = UID[1];
                                    UI2[2] = UID[2];
                                    UI2[3] = UID[3];
                                    FLASH_W(FLASH_ADDR3, UI2[0], UI2[1], UI2[2], UI2[3]);
                                    OLED_ShowString(1, 1, "  Add Card 3 OK ");
                                    Buzzer1();
                                    WaitCardOff();
                                    OLED_Clear();
                                    break;
                                case 3:
                                    UI3[0] = UID[0];
                                    UI3[1] = UID[1];
                                    UI3[2] = UID[2];
                                    UI3[3] = UID[3];
                                    FLASH_W(FLASH_ADDR4, UI3[0], UI3[1], UI3[2], UI3[3]);
                                    OLED_ShowString(1, 1, "  Add Card 4 OK ");
                                    Buzzer1();
                                    WaitCardOff();
                                    OLED_Clear();
                                    break;
                                case 4:
                                    OLED_ShowString(1, 1, "Card max!!");
                                    Buzzer1();
                                    Delay_ms(500);
                                    OLED_Clear();
                                    break;
                                case 5:
                                    OLED_ShowString(1, 1, "Card exists!");
                                    Buzzer1();
                                    Delay_ms(500);
                                    OLED_Clear();
                                    break;
                            }
                        }
                    }
                }
				
				while(flag_deletecard==1)	//删卡模式
				{
					if (isFirstEnter) {
                        OLED_Clear();  // 清屏
                        isFirstEnter = 0;
                    }
					OLED_ShowString(1,1,"  Delete Card:  ");
					KeyNum = Key_Scan();
					
					if(KeyNum==4)	//按下4键退出删卡模式，进入寻卡模式
					{
						flag_scan=0;
						flag_addcard=0;
						flag_deletecard=0;
						
						OLED_Clear();

						inPasswordMode = 2;
						isFirstEnter = 1;
						rfid_display_flag = 0;
					}
					
					
					if(KeyNum==1)	//在删卡模式下按下1键，选择要删除的卡片，卡片序列增加
					{
						select++;
						if(select>=4 || select<1)select=0;
					}
					if(KeyNum==2)	//在删卡模式下按下2键，选择要删除的卡片，卡片序列减少
					{
						select--;
						if(select>=4 || select<1)select=0;
					}
					
					
					switch (select)
					{
						case 0:
						{
							OLED_ShowString(2,1," Delete Card 1 ?");
							if(KeyNum==3)	//在删卡模式下按下3键，删除对应的卡片
							{
								FLASH_Clear(FLASH_ADDR1);	
								UI0[0]=0xFF;
								UI0[1]=0xFF;
								UI0[2]=0xFF;
								UI0[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 1 OK ");
								Buzzer1();	//删除成功后蜂鸣器响一声
								Delay_ms(1500);
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						case 1:
						{
							OLED_ShowString(2,1," Delete Card 2 ?");
							if(KeyNum==3)
							{
								FLASH_Clear(FLASH_ADDR2);
								UI1[0]=0xFF;
								UI1[1]=0xFF;
								UI1[2]=0xFF;
								UI1[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 2 OK ");
								Buzzer1();
								Delay_ms(1500);
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						case 2:
						{
							OLED_ShowString(2,1," Delete Card 3 ?");
							if(KeyNum==3)
							{
								FLASH_Clear(FLASH_ADDR3);
								UI2[0]=0xFF;
								UI2[1]=0xFF;
								UI2[2]=0xFF;
								UI2[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 3 OK ");
								Buzzer1();
								Delay_ms(1500);;
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						case 3:
						{
							OLED_ShowString(2,1," Delete Card 4 ?");
							if(KeyNum==3)
							{
								FLASH_Clear(FLASH_ADDR4);
								UI3[0]=0xFF;
								UI3[1]=0xFF;
								UI3[2]=0xFF;
								UI3[3]=0xFF;
								OLED_ShowString(2,1,"Clear Card 4 OK ");
								Buzzer1();
								Delay_ms(1500);
							}
							else if(KeyNum==4){
								OLED_Clear();
								break;
							}
						}break;
						default:break;
					}
				}
            }
			else if (inPasswordMode == 3) {  // 蓝牙模式
				if (isFirstEnter) {
                    OLED_Clear();  // 清屏
                    isFirstEnter = 0;
                    OLED_ShowString(1, 1, "Connecting...");
                }
                if (passwordReceived && !blue_tooth_mode) {
                    // 比较接收到的密码是否正确
                    if (strcmp(receivedPassword, "1234") == 0) {
                        // 密码正确
                        OLED_Clear();
                        OLED_ShowString(1, 1, "Yes");
                        // 显示 Opening door
                        OLED_ShowString(2, 1, "Opening door...");
						// 关闭USART2接收中断
						USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); 
						blue_tooth_mode = 1;	
                  }
					//密码错误
					else {
						// 密码错误
						OLED_Clear();
						OLED_ShowString(1, 1, "No");
						error_count ++;
						// 保持 TIM2 中断关闭
						Delay_ms(2000);
						OLED_Clear();
						OLED_ShowString(1, 1, "Pls try again!");
						uint8_t err_left = MAX_ERROR_COUNT-error_count;
						OLED_ShowNum(2, 1, err_left,1);
						OLED_ShowString(2, 2, " times left");
						TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
					}

                    // 更新last_receivedPassword（可选，根据需求）
                    strncpy(last_receivedPassword, receivedPassword, BUFFER_SIZE);
                    last_receivedPassword[BUFFER_SIZE - 1] = '\0'; // 确保字符串结束

                    // 重置标志
                    passwordReceived = false;

                    // 清空密码缓冲区，准备下次接收
                    ClearPasswordBuffer();

			}
				else if(blue_tooth_mode == 1){
					rotate90_degrees();
				}
				else if(blue_tooth_mode == 2){
											
					Delay_ms(1500);
					// 可选：显示开锁完成
					OLED_ShowString(3, 1, "Door Opened");
					Delay_ms(2000);
					OLED_Clear();

					blue_tooth_mode = 3;
					
				}
				else if(blue_tooth_mode == 3){
					inPasswordMode = 0;
                    passwordReceived = false;
					blue_tooth_mode = 0;
					OLED_Clear();
					displayMainMenu();
					// 重新开启 TIM2 中断
					TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
					USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能USART2接收中断
					isFirstEnter=1;
				}

		}
        }
		

//		count = 3;
		if(clear_flag == 1 ){
			if(board_turn_flag_cw  == 1 && !board_turn_flag_ccw){//如果超时发现门没关
//				TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // 使能USART2接收中断
//				board_turn_flag_cw = 1;//标志位置1 代表已经关门
				rotate90_degrees_reverse();//顺时针
			}
			else OLED_Clear();
			
			if(KeyNum){
				clear_flag = 0;
				reset_flag = 1;
				//重置标志位
				inPasswordMode = 0;
			}

		}
		else if(error_count <MAX_CLOSE_COUNT)OLED_ShowNum(4, 15, count, 2);
		
		if(count>=MAX_CLOSE_COUNT && error_count <MAX_ERROR_COUNT){ //超过时间点 或者 按下按键
			count =0; 
			clear_flag = 1;
			if(board_turn_flag_cw  == 1 && !board_turn_flag_ccw && count>=MAX_CLOSE_COUNT){//如果超时发现门没关
//				TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // 使能USART2接收中断
//				board_turn_flag_cw = 1;//标志位置1 代表已经关门
				rotate90_degrees_reverse();//关门
		}
		}
		else if(count >=MAX_CLOSE_COUNT && error_count>=MAX_ERROR_COUNT){ // 错误次数超过5次 60秒后重置
			error_count = 0;
			count = 0;
			displayMainMenu();
		}
		else {
			if(KeyNum) count = 0; //如果按下按键清零
		}
    }
}


// 检查RFID卡片的函数，读取卡片ID
void RFID_Check()									
{
	if(!rfid_mode){
		cardnumber = Rc522Test();	// 获取卡片编号
		if (isFirstEnter) {
			OLED_Clear();  // 清屏
			isFirstEnter = 0;
		}	
		if(cardnumber == 0)			// 如果卡片无效（返回0），则显示错误信息
		{
			OLED_ShowString(1,1,"   Error card   ");
			Buzzer_Alarm();			// 发出警报音
			WaitCardOff();			// 等待卡片移开
		}
		// 如果卡片编号是1到4，表示有效卡片
		else if((cardnumber == 1 || cardnumber == 2 || cardnumber == 3 || cardnumber == 4))
		{
			OLED_Clear();
			OLED_ShowString(1,1,"The CardID is:  ");
			OLED_ShowNum(1,15,cardnumber,2);
			OLED_ShowString(3,1,"opening...");
			Delay_ms(500);
			rfid_mode = 1; // 开门标志位
		}
		else 		OLED_ShowString(3,3,"Scanning...");
	}
}

// 从Flash中读取卡片ID（用于存储卡片ID的闪存地址）
void Read_Card()
{
    UI0[0] = FLASH_R(FLASH_ADDR1);
    UI0[1] = FLASH_R(FLASH_ADDR1 + 2);
    UI0[2] = FLASH_R(FLASH_ADDR1 + 4);
    UI0[3] = FLASH_R(FLASH_ADDR1 + 6);
	
    UI1[0] = FLASH_R(FLASH_ADDR2);
    UI1[1] = FLASH_R(FLASH_ADDR2 + 2);
    UI1[2] = FLASH_R(FLASH_ADDR2 + 4);
    UI1[3] = FLASH_R(FLASH_ADDR2 + 6);
	
    UI2[0] = FLASH_R(FLASH_ADDR3);
    UI2[1] = FLASH_R(FLASH_ADDR3 + 2);
    UI2[2] = FLASH_R(FLASH_ADDR3 + 4);
    UI2[3] = FLASH_R(FLASH_ADDR3 + 6);
	
    UI3[0] = FLASH_R(FLASH_ADDR4);
    UI3[1] = FLASH_R(FLASH_ADDR4 + 2);
    UI3[2] = FLASH_R(FLASH_ADDR4 + 4);
    UI3[3] = FLASH_R(FLASH_ADDR4 + 6);
}

// USART2中断服务程序
void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET && inPasswordMode==3) {
        char receivedChar = USART_ReceiveData(USART2);  // 读取接收到的数据

        // 检查是否接收到结束符
        if (receivedChar == PASSWORD_END_CHAR) {
            receivedPassword[passwordIndex] = '\0';  // 添加字符串结束符
            passwordReceived = true;                 // 设置标志
            passwordIndex = 0;                       // 重置索引
        } else {
            if (passwordIndex < BUFFER_SIZE - 1) {   // 检查缓冲区是否有足够空间
                receivedPassword[passwordIndex++] = receivedChar;  // 存储接收到的字符
            } else {
                // 缓冲区已满，清空缓冲区
                ClearPasswordBuffer();
                // 在OLED上显示错误信息
                OLED_ShowString(4, 1, "Password Too Long!");
            }
        }
    }
}

void TIM2_IRQHandler(void)//
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
		if(key_err==0&&stop_flag==0)//如果没有检测到按键按下
		count++;//定时计数
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}


