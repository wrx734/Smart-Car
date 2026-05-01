#include "stm32f10x.h"                  // Device header

void PWM_Init(void)							//初始化
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;							//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;                 //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;             //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 200 - 1;		//ARR                   //计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//PSC               //预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;                        //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);                         //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure2;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure2.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure2.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure2.TIM_Pulse = 0;		//CCR
	TIM_OC2Init(TIM2, &TIM_OCInitStructure2);
	
	TIM_Cmd(TIM2, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)			//定时器2,通道1,右轮
{
	TIM_SetCompare1(TIM2, Compare);
}

void PWM_SetCompare2(uint16_t Compare)			//定时器2,通道2,左轮
{
	TIM_SetCompare2(TIM2, Compare);
}
