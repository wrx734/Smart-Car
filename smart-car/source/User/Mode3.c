#include "stm32f10x.h"                  // Device header
#include "OLED.h"

/*模式3*/
extern uint8_t KeyNum;
uint8_t Time;
uint8_t TimeRunFlag;

void Mode3_Init(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "[Mode3]");
	OLED_ShowString(2, 1, "Time:");
	OLED_ShowNum(2, 6, Time, 3);
}

void Mode3_Loop(void)
{
	if (KeyNum == 2)
	{
		TimeRunFlag = !TimeRunFlag;
	}
	OLED_ShowNum(2, 6, Time, 3);
}

void Mode3_Exit(void)
{
	TimeRunFlag = 0;
	Time = 0;
}

void Mode3_Tick(void)
{
	static uint16_t Count0;
	Count0 ++;
	if (Count0 >= 1000)
	{
		Count0 = 0;
		
		if (TimeRunFlag)
		{
			Time ++;
		}
	}
}
