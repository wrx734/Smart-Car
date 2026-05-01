#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t Key_Num;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP , ENABLE);
	//要把PC14和PC15设置为普通GPIO功能需要先把原本默认的备份域和外部时钟功能关闭
	PWR_BackupAccessCmd(DISABLE);			
	RCC_LSEConfig(RCC_LSE_OFF);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	if (Key_Num)
	{
		Temp = Key_Num;
		Key_Num = 0;
		return Temp;
	}
	return 0;
}

uint8_t Key_GetState(void)
{
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0)
	{
		return 1;
	}
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0)
	{
		return 2;
	}
	return 0;
}

void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;
	
	Count ++;
	if (Count >= 20)
	{
		Count = 0;
		
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if (CurrState == 0 && PrevState != 0)
		{
			Key_Num = PrevState;
		}
	}
}
