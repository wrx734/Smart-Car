#include "stm32f10x.h"                  // Device header

#define LED_GPIO_RCC RCC_APB2Periph_GPIOC
#define LED_GPIO_PORT GPIOC
#define LED1_PIN GPIO_Pin_13
#define LED2_PIN GPIO_Pin_14
#define LED3_PIN GPIO_Pin_15
#define LED_GPIO_PINS (LED1_PIN | LED2_PIN | LED3_PIN)

uint8_t LED1_Mode;
uint8_t LED2_Mode;
uint8_t LED3_Mode;

uint16_t LED1_Count;
uint16_t LED2_Count;
uint16_t LED3_Count;

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(LED_GPIO_RCC, ENABLE);
    
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
	GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PINS);
}

void LED1_SetMode(uint8_t Mode)
{
	if (Mode != LED1_Mode)
	{
		LED1_Mode = Mode;
		LED1_Count = 0;
	}
}

void LED2_SetMode(uint8_t Mode)
{
	if (Mode != LED2_Mode)
	{
		LED2_Mode = Mode;
		LED2_Count = 0;
	}
}

void LED3_SetMode(uint8_t Mode)
{
	if (Mode != LED3_Mode)
	{
		LED3_Mode = Mode;
		LED3_Count = 0;
	}
}

void LED1_ON(void)
{
	GPIO_ResetBits(LED_GPIO_PORT, LED1_PIN);
}

void LED1_OFF(void)
{
	GPIO_SetBits(LED_GPIO_PORT, LED1_PIN);
}

void LED2_ON(void)
{
	GPIO_ResetBits(LED_GPIO_PORT, LED2_PIN);
}

void LED2_OFF(void)
{
	GPIO_SetBits(LED_GPIO_PORT, LED2_PIN);
}

void LED3_ON(void)
{
	GPIO_ResetBits(LED_GPIO_PORT, LED3_PIN);
}

void LED3_OFF(void)
{
	GPIO_SetBits(LED_GPIO_PORT, LED3_PIN);
}

void LED_Tick(void)
{
	if (LED1_Mode == 0)
	{
		LED1_OFF();
	}
	else if (LED1_Mode == 1)
	{
		LED1_ON();
	}
	else if (LED1_Mode == 2)
	{
		LED1_Count ++;
		LED1_Count %= 1000;
        
		if (LED1_Count < 500)
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	else if (LED1_Mode == 3)
	{
		LED1_Count ++;
		LED1_Count %= 100;
        
		if (LED1_Count < 50)
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	else if (LED1_Mode == 4)
	{
		LED1_Count ++;
		LED1_Count %= 1000;
        
		if (LED1_Count < 100)
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
    
	if (LED2_Mode == 0)
	{
		LED2_OFF();
	}
	else if (LED2_Mode == 1)
	{
		LED2_ON();
	}
	else if (LED2_Mode == 2)
	{
		LED2_Count ++;
		LED2_Count %= 1000;
        
		if (LED2_Count < 500)
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	else if (LED2_Mode == 3)
	{
		LED2_Count ++;
		LED2_Count %= 100;
        
		if (LED2_Count < 50)
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	else if (LED2_Mode == 4)
	{
		LED2_Count ++;
		LED2_Count %= 1000;
        
		if (LED2_Count < 100)
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}

	if (LED3_Mode == 0)
	{
		LED3_OFF();
	}
	else if (LED3_Mode == 1)
	{
		LED3_ON();
	}
	else if (LED3_Mode == 2)
	{
		LED3_Count ++;
		LED3_Count %= 1000;
        
		if (LED3_Count < 500)
		{
			LED3_ON();
		}
		else
		{
			LED3_OFF();
		}
	}
	else if (LED3_Mode == 3)
	{
		LED3_Count ++;
		LED3_Count %= 100;
        
		if (LED3_Count < 50)
		{
			LED3_ON();
		}
		else
		{
			LED3_OFF();
		}
	}
	else if (LED3_Mode == 4)
	{
		LED3_Count ++;
		LED3_Count %= 1000;
        
		if (LED3_Count < 100)
		{
			LED3_ON();
		}
		else
		{
			LED3_OFF();
		}
	}
}
