#include "stm32f10x.h"                  // Device header

/**
  * 函    数：光敏传感器初始化
  * 参    数：无
  * 返 回 值：无
  */
void LightSensor_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/*GPIO初始化*/
	
	GPIO_InitTypeDef GPIO_InitStructure2;						//开启GPIOA的时钟
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure2.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 ;
    GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure2);						//将PA 2 3 5 8 9 11 12引脚初始化为上拉输入
}

/**
  * 函    数：获取当前光敏传感器输出的高低电平
  * 参    数：无
  * 返 回 值：光敏传感器输出的高低电平，范围：0/1
  */

uint8_t LightSensor_Get_R4(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);			//返回PA12输入寄存器的状态 右4
}

uint8_t LightSensor_Get_R3(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);			//返回PA11输入寄存器的状态	右3
}

uint8_t LightSensor_Get_R2(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);			//返回PA10输入寄存器的状态	右2
}

uint8_t LightSensor_Get_R1(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);			//返回PA9输入寄存器的状态	右1
}


uint8_t LightSensor_Get_L1(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);			//返回PA8输入寄存器的状态 左1
}

uint8_t LightSensor_Get_L2(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);			//返回PA5输入寄存器的状态	左2
}

uint8_t LightSensor_Get_L3(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3);			//返回PA3输入寄存器的状态	左3
}

uint8_t LightSensor_Get_L4(void)
{
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);			//返回PA2输入寄存器的状态	左4
}



int16_t Get_Turn_Value(void)			//获取转向值
{
	int Senor[8] = {0};							//传感器返回值存放数组
	float weight[8] = {-7,-5,-3,-1,1,3,5,7};	//设置各传感器权重
	float sum_weight = 0;						//用来存放数值乘权重后的结果
	int sum_sensor = 0;							//用来记录有变化的传感器的个数
	float Error,last_error;						//存放转向值和上次转向值
	
	Senor[0] = LightSensor_Get_L4();			//将传感器传回数据存放到数组里
	Senor[1] = LightSensor_Get_L3();
	Senor[2] = LightSensor_Get_L2();
	Senor[3] = LightSensor_Get_L1();
	Senor[4] = LightSensor_Get_R1();
	Senor[5] = LightSensor_Get_R2();
	Senor[6] = LightSensor_Get_R3();
	Senor[7] = LightSensor_Get_R4();
	
	for(int i=0;i<8;i++)				//循环遍历数组
	{
		sum_weight += Senor[i] * weight[i]; 	//传感器数值乘对应权重
		sum_sensor += Senor[i];					//记录有变化的传感器个数
	}
	
	Error = sum_weight/sum_sensor;		//计算得到转向值
	
	if(sum_sensor != 0)					//转向值为零时返回上次的转向值,用于应对转弯不及时脱离循迹线的情况
	{
		last_error = Error;
	}
	else if(sum_sensor == 0)
	{
		Error = last_error;
	}
	
	return Error;
	
}


//检测是否经过黑线
uint8_t LightSensor_Check_AllBlack(void)
{
    return (LightSensor_Get_L4() &
            LightSensor_Get_L3() &
            LightSensor_Get_L2() &
            LightSensor_Get_L1() &
            LightSensor_Get_R1() &
            LightSensor_Get_R2() &
            LightSensor_Get_R3() &
            LightSensor_Get_R4());
}
