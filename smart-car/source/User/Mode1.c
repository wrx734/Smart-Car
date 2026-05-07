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

/*模式1*/

int16_t Location, Speed_Base = 50; // 用于最后速度输出的基准速度
/*调整基准速度后需要重新调参,
提高速度,减小KP,增加KD,减小KI
降低速度,增大KP,减小KD,增大KI
*/

int16_t Speed_L, Speed_R;                     // 存放获取的实际速度用于后续计算
float k = 0.3;                                // 用于进行速度滤波的变量
int16_t filt_Speed_L, filt_Speed_R;           // 存放滤波后要传给pid的速度值
int16_t last_file_Speed_L, last_file_Speed_R; // 存放用于滤波计算的上一次速度值

uint8_t P = 0; // 标志位用于按键控制开关

/*定义PID结构体变量*/
// 内环 速度环
PID_t Inner_L = {
    // 内环PID结构体变量，定义的时候同时给部分成员赋初值
    .Kp     = 1.7,  // 比例项权重
    .Ki     = 0.15, // 积分项权重
    .Kd     = 0,    // 微分项权重
    .OutMax = 100,  // 输出限幅的最大值
    .OutMin = -100, // 输出限幅的最小值
};

PID_t Inner_R = {
    // 内环PID结构体变量，定义的时候同时给部分成员赋初值
    .Kp     = 1.3,  // 比例项权重
    .Ki     = 0.12, // 积分项权重
    .Kd     = 0,    // 微分项权重
    .OutMax = 100,  // 输出限幅的最大值
    .OutMin = -100, // 输出限幅的最小值
};

// 外环 转向环
PID_t Outer = {
    // 外环PID结构体变量，定义的时候同时给部分成员赋初值
    .Kp     = 1,   // 比例项权重
    .Ki     = 0,   // 积分项权重
    .Kd     = 1.5, // 微分项权重
    .OutMax = 50,  // 输出限幅的最大值
    .OutMin = -50, // 输出限幅的最小值
};

void Mode1_Init(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "[Mode1]", OLED_8X16);
    OLED_Update();
}

void Mode1_Loop(void)
{
    while (1) {

        if (Key_Check(KEY_2, KEY_DOWN)) // 按键按下时标志位取反,用于控制程序开关
        {
            P = !P;
        }
        OLED_ShowNum(0, 48, P, 1, OLED_8X16); // 把标志位状态打印在oled显示屏上

        OLED_ShowNum(0, 0, L4, 1, OLED_8X16); // 把灰度传感器状态打印在oled显示屏上
        OLED_ShowNum(16, 0, L3, 1, OLED_8X16);
        OLED_ShowNum(32, 0, L2, 1, OLED_8X16);
        OLED_ShowNum(48, 0, L1, 1, OLED_8X16);
        OLED_ShowNum(64, 0, R1, 1, OLED_8X16);
        OLED_ShowNum(80, 0, R2, 1, OLED_8X16);
        OLED_ShowNum(96, 0, R3, 1, OLED_8X16);
        OLED_ShowNum(112, 0, R4, 1, OLED_8X16);

        OLED_ShowSignedNum(0, 16, Speed_L, 4, OLED_8X16); // 把实测速度值打印在oled显示屏上
        OLED_ShowSignedNum(48, 16, Speed_R, 4, OLED_8X16);

        OLED_Update();

        // Motor_Enable();
        // Motor_SetSpeed(199, 199); // 电机以实际速度运行
        // Motor_SetSpeed(100,100); // 电机以100的速度运行
        // Motor_SetSpeed(50,50); // 电机以100的速度运行
    }
}

void Mode1_Exit(void)
{
    P                 = 0; // 退出模式1时标志位复位
    last_file_Speed_L = 0;
    last_file_Speed_R = 0;
    Motor_SetSpeed(0, 0); // 电机以0的速度运行
    Motor_Disable();      // 电机禁用
    OLED_Clear();
}

void Mode1_Tick(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {

        Key_Tick();

        /*定义静态变量（默认初值为0，函数退出后保留值和存储空间）*/
        static uint16_t Count1, Count2;     // 分别用于内环和外环的计次分频
        static uint16_t StopDelayCount = 0; // 用于延时停车

        if (P == 1) // 判断,标志位为1是开启开启电机和循迹程序,否则关闭,速度置为0
        {
            // 检测到横线停车
            if (LightSensor_Check_AllBlack() == 1) {
                StopDelayCount++;
                if (StopDelayCount >= 60) { // 假设中断周期1ms，延时60ms
                    StopDelayCount = 0;
                    Motor_SetSpeed(0, 0);
                    Motor_Disable();
                    P        = 0;
                    NextMode = 2; // 切换到模式2
                }
            } else {
                /*内环计次分频*/
                Count1++;        // 计次自增
                if (Count1 >= 5) // 如果计次5次，则if成立，即if每隔5ms进一次
                {
                    Count1 = 0; // 计次清零，便于下次计次

                    // 将内环目标值设为基准速度,用于内环调参
                    //  Inner_L.Target = 0; // 传给内环的速度值,基础速度加上外环输出的速度值
                    //  Inner_R.Target = 50;

                    /*获取实际速度值和实际位置值*/
                    /*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
                    /*此值正比于速度，所以可以表示速度，但它的单位并不是速度的标准单位*/
                    /*此处每隔40ms获取一次计次值增量，电机旋转一周的计次值增量约为408*/
                    /*因此如果想转换为标准单位，比如转/秒*/
                    /*则可将此句代码改成Speed = Encoder_Get() / 408.0 / 0.04;*/
                    // Speed_L = Get_Speed_L(); // 获取编码器增量，得到实际速度
                    // Speed_R = Get_Speed_R();

                    Speed_L = Encoder_L_Get();
                    Speed_R = Encoder_R_Get();

                    Serial_Printf("%d,%d\n", Speed_L, Speed_R);

                    /*以下进行内环PID控制*/

                    // // 滤波处理
                    filt_Speed_L      = k * Speed_L + (1 - k) * last_file_Speed_L;
                    filt_Speed_R      = k * Speed_R + (1 - k) * last_file_Speed_R;
                    last_file_Speed_L = filt_Speed_L;
                    last_file_Speed_R = filt_Speed_R;

                    /*内环获取实际值*/
                    Inner_L.Actual = filt_Speed_L; // 内环为速度环，实际值为速度值
                    Inner_R.Actual = filt_Speed_R;

                    /*PID计算及结构体变量值更新*/
                    PID_Update(&Inner_L); // 调用封装好的函数，一步完成PID计算和更新
                    PID_Update(&Inner_R);
                    /*内环执行控制*/
                    /*内环输出值给到电机PWM*/
                    Motor_Enable();
                    Motor_SetSpeed(Inner_L.Out, Inner_R.Out);
                }

                /*外环计次分频*/
                Count2++;         // 计次自增
                if (Count2 >= 10) // 如果计次10次，则if成立，即if每隔10ms进一次
                {
                    Count2 = 0; // 计次清零，便于下次计次

                    /*以下进行外环PID控制*/
                    Outer.Target = 0; // 外环目标值,循迹线在中心时为零

                    /*外环获取实际值*/
                    Outer.Actual = Get_Turn_Value(); // 外环实际值,传感器数值加权求平均值

                    /*PID计算及结构体变量值更新*/
                    PID_Update(&Outer); // 调用封装好的函数，一步完成PID计算和更新

                    /*外环执行控制*/
                    /*外环的输出值作用于内环的目标值，组成串级PID结构*/
                    Inner_L.Target = Speed_Base + Outer.Out; // 传给内环的速度值,基础速度加上外环输出的速度值
                    Inner_R.Target = Speed_Base - Outer.Out;
                }
            }
        } else {
            Motor_SetSpeed(0, 0); // 电机以0的速度运行
        }
    }
}
