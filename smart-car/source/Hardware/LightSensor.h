#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#define L1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)
#define L2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)
#define L3 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)
#define L4 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)
#define R4 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)   
#define R3 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)
#define R2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10)
#define R1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9)


void LightSensor_Init(void);

uint8_t LightSensor_Get_R4(void);
uint8_t LightSensor_Get_R3(void);
uint8_t LightSensor_Get_R2(void);
uint8_t LightSensor_Get_R1(void);
uint8_t LightSensor_Get_L1(void);
uint8_t LightSensor_Get_L2(void);
uint8_t LightSensor_Get_L3(void);
uint8_t LightSensor_Get_L4(void);

int16_t Get_Turn_Value(void);

uint8_t LightSensor_Check_AllBlack(void);

#endif
