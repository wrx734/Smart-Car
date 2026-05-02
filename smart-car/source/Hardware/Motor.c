#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)				//初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB时钟

    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_3| GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    PWM_Init();
}



void Motor_SetSpeed(int16_t Speed_L,int16_t Speed_R)		//速度设置
{
	if (Speed_L >= 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_4);
		GPIO_ResetBits(GPIOB, GPIO_Pin_4);
		PWM_SetCompare1(Speed_L);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_3);
		GPIO_SetBits(GPIOB, GPIO_Pin_4);
		PWM_SetCompare1(-Speed_L);
	}
	
	if (Speed_R >= 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		PWM_SetCompare2(Speed_R);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		PWM_SetCompare2(-Speed_R);
	}
}


void Motor_Enable(void)					//使能
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void Motor_Disable(void)					//失能
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}