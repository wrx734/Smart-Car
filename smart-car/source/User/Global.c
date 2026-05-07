#include "stm32f10x.h" // Device header
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
#include "Global.h"

/*全局模式*/
/* ==================== 全局变量定义 ==================== */
uint8_t CurrMode     = 0; // 默认未进入任何模式
uint8_t NextMode     = 1; // 默认进入模式1
uint8_t KeyNum       = 0; // 默认无按键
uint8_t SystemReady  = 0; // 系统未就绪
uint8_t MotorEnabled = 0; // 电机禁用

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
    PWM_Init();

    SystemReady = 1; // 初始化完成
}

void Global_Loop(void)
{
    if (Key_Check(KEY_5, KEY_DOWN)) {
        NextMode++;
        if (NextMode > 2) {
            NextMode = 1;
        }
    }
}

void Global_Tick(void)
{
}
