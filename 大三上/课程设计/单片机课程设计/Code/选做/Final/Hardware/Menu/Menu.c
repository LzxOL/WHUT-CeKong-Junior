#include "stm32f10x.h"                  // Device header
#include "OLED.h"

void displayMainMenu(void)
{
    OLED_ShowString(1, 3, "Door Locked");
    OLED_ShowString(2, 1, "1.Password");
    OLED_ShowString(3, 1, "2.RFID");
    OLED_ShowString(4, 1, "3.BlueTooth");
}
