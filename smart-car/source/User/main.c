#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Key.h"
#include "LightSensor.h"
#include "Encoder.h"
#include "Serial.h"
#include "Timer.h"
#include "PID.h"

//int16_t AX, AY, AZ, GX, GY, GZ;			//定义用于存放各个数据的变量
uint8_t KeyNum;								//定义用于存放按键的变量
int16_t Speed_L,Speed_R;					//存放获取的实际速度用于后续计算
float k = 0.3;								//用于进行速度滤波的变量
int16_t filt_Speed_L,filt_Speed_R;			//存放滤波后要传给pid的速度值
int16_t last_file_Speed_L,last_file_Speed_R;//存放用于滤波计算的上一次速度值
int16_t Location,Speed_Base = 100;			//用于最后速度输出的基准速度
uint8_t P=0;								//标志位用于按键控制开关

/*定义PID结构体变量*/
//内环 速度环
PID_t Inner_L = {					//内环PID结构体变量，定义的时候同时给部分成员赋初值
	.Kp = 1.0,					//比例项权重
	.Ki = 0.1,					//积分项权重
	.Kd = 0,					//微分项权重
	.OutMax = 190,				//输出限幅的最大值
	.OutMin = -190,				//输出限幅的最小值
};

PID_t Inner_R = {					//内环PID结构体变量，定义的时候同时给部分成员赋初值
	.Kp = 1.0,					//比例项权重
	.Ki = 0.1,					//积分项权重
	.Kd = 0,					//微分项权重
	.OutMax = 190,				//输出限幅的最大值
	.OutMin = -190,				//输出限幅的最小值
};

//外环 转向环
PID_t Outer = {					//外环PID结构体变量，定义的时候同时给部分成员赋初值
	.Kp = 5,					//比例项权重
	.Ki = 0,					//积分项权重
	.Kd = 0,					//微分项权重
	.OutMax = 90,				//输出限幅的最大值
	.OutMin = -90,				//输出限幅的最小值
};



int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Motor_Init();		//电机驱动初始化
	Key_Init();			//按键初始化
	LightSensor_Init();	//灰度传感器初始化
	Encoder_Init();		//测速编码器初始化	
	MPU6050_Init();		//MPU6050初始化
	Serial_Init();		//串口初始化
	Timer_Init();		//定时器初始化
	

	
while (1)
	{
		KeyNum = Key_GetNum();//获取按键状态
		
		if (KeyNum == 1)	//按键按下时标志位取反,用于控制程序开关
		{
			P = !P;
		}

		
		OLED_ShowNum(1,1,L4,1);		//把灰度传感器状态打印在oled显示屏上
		OLED_ShowNum(1,3,L3,1);
		OLED_ShowNum(1,5,L2,1);
		OLED_ShowNum(1,7,L1,1);	
		OLED_ShowNum(1,9,R1,1);
		OLED_ShowNum(1,11,R2,1);
		OLED_ShowNum(1,13,R3,1);	
		OLED_ShowNum(1,15,R4,1);
		
		Serial_Printf("%d,%d\n",Speed_L,Speed_R);	//把实测速度值传给串口
		
		
		
		}
}




void TIM1_UP_IRQHandler(void)				//设置定时中断,用于按键检测
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{

		
		Key_Tick();
		
		
		/*定义静态变量（默认初值为0，函数退出后保留值和存储空间）*/
		static uint16_t Count1, Count2;		//分别用于内环和外环的计次分频
		
		if(P == 1)		//判断,标志位为1是开启开启电机和循迹程序,否则关闭,速度置为0
		{
				/*内环计次分频*/
		Count1 ++;				//计次自增
		if (Count1 >= 40)		//如果计次40次，则if成立，即if每隔40ms进一次
		{
			Count1 = 0;			//计次清零，便于下次计次
			
			
			
			/*获取实际速度值和实际位置值*/
			/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
			/*此值正比于速度，所以可以表示速度，但它的单位并不是速度的标准单位*/
			/*此处每隔40ms获取一次计次值增量，电机旋转一周的计次值增量约为408*/
			/*因此如果想转换为标准单位，比如转/秒*/
			/*则可将此句代码改成Speed = Encoder_Get() / 408.0 / 0.04;*/
			Speed_L = Get_Speed_L();		//获取编码器增量，得到实际速度
			Speed_R = Get_Speed_R();
			
			
//			Location += Speed;			//实际速度累加，得到实际位置
			
			/*以下进行内环PID控制*/
			
			//滤波处理
			filt_Speed_L = k*Speed_L + (1-k)*last_file_Speed_L ;
			filt_Speed_R = k*Speed_R + (1-k)*last_file_Speed_R ;
			last_file_Speed_L = filt_Speed_L;
			last_file_Speed_R = filt_Speed_R;
			
			/*内环获取实际值*/
			Inner_L.Actual = filt_Speed_L;		//内环为速度环，实际值为速度值
			Inner_R.Actual = filt_Speed_R;
			
			/*PID计算及结构体变量值更新*/
			PID_Update(&Inner_L);			//调用封装好的函数，一步完成PID计算和更新
			PID_Update(&Inner_R);	
			/*内环执行控制*/
			/*内环输出值给到电机PWM*/
			Motor_SetSpeed(Inner_L.Out,Inner_R.Out);
			
		}
		
		/*外环计次分频*/
		Count2 ++;				//计次自增
		if (Count2 >= 40)		//如果计次40次，则if成立，即if每隔40ms进一次
		{
			Count2 = 0;			//计次清零，便于下次计次
			
			/*以下进行外环PID控制*/
			Outer.Target = 0;		//外环目标值,循迹线在中心时为零
			
			/*外环获取实际值*/
			Outer.Actual = Get_Turn_Value();	//外环实际值,传感器数值加权求平均值
			
			/*PID计算及结构体变量值更新*/
			PID_Update(&Outer);			//调用封装好的函数，一步完成PID计算和更新
			
			/*外环执行控制*/ 
			/*外环的输出值作用于内环的目标值，组成串级PID结构*/
			Inner_L.Target = Speed_Base + Outer.Out;	//传给内环的速度值,基础速度加上外环输出的速度值
			Inner_R.Target = Speed_Base - Outer.Out;
		}
	}
	else
	{
		Motor_SetSpeed(0,0);
	}
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}





//			OLED_ShowNum(1,1,L4,1);
//			OLED_ShowNum(1,3,L3,1);
//			OLED_ShowNum(1,5,L2,1);
//			OLED_ShowNum(1,7,L1,1);	
//			OLED_ShowNum(1,9,R1,1);
//			OLED_ShowNum(1,11,R2,1);
//			OLED_ShowNum(1,13,R3,1);	
//			OLED_ShowNum(1,15,R4,1);

				
//			MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		//获取MPU6050的数据
//			OLED_ShowSignedNum(3, 1, AX, 4);					//OLED显示数据
//			OLED_ShowSignedNum(3, 6, AY, 4);
//			OLED_ShowSignedNum(3, 11, AZ, 4);
//			OLED_ShowSignedNum(4, 1, GX, 4);
//			OLED_ShowSignedNum(4, 6, GY, 4);
//			OLED_ShowSignedNum(4, 11, GZ, 4);
			



