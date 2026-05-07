#include "stm32f10x.h"                  // Device header
#include "OLED.h"

/*模式2*/
extern uint8_t KeyNum;
uint8_t Count;

void Mode2_Init(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "[Mode2]", OLED_8X16);
    OLED_Update();
}

void Mode2_Loop(void)
{

}

void Mode2_Exit(void)
{

}
