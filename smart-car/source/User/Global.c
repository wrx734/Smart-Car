#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "LightSensor.h"
#include "Encoder_R.h"
#include "Encoder_L.h"
#include "Serial.h"
#include "Timer.h"
#include "PID.h"
#include "PWM.h"


/*全局模式*/
extern uint8_t NextMode;
uint8_t KeyNum;

void Global_Init(void)
{
	OLED_Init();
	Key_Init();
	Motor_Init();
	LightSensor_Init();
	Encoder_L_Init();
	Encoder_R_Init();
	Serial_Init();
	Timer_Init();
	PID_Init();
	PWM_Init();
}

void Global_Loop(void)
{
    if (Key_Check(KEY_5, KEY_DOWN)) 
	{
        NextMode ++;
		if (NextMode > 2)
		{
			NextMode = 1;
		}
    }
}

void Global_Tick(void)
{
	
}
