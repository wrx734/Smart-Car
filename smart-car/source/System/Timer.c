#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);				//定时器时钟
	
	TIM_InternalClockConfig(TIM4);										//选择定时器时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;					//定义结构体变量 定时器初始化设置
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;     //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;                    //计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;                   //预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);                 //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	
	TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;								//NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	
	TIM_Cmd(TIM4, ENABLE);							//定时器使能
}

/*
void TIM4_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
*/
