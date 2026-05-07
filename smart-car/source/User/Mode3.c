#include "stm32f10x.h" // Device header
#include "OLED.h"
#include "Key.h"
#include "Global.h"


/*模式3*/

void Mode3_Init(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "[Mode3]", OLED_8X16);
    OLED_Update();
}

void Mode3_Loop(void)
{

}

void Mode3_Exit(void)
{

}

void Mode3_Tick(void)
{
	
}
