#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "stm32f10x.h"

/* ==================== 全局模式控制 ==================== */
extern uint8_t CurrMode; // 当前模式 (1:循迹, 2:模式2, 3:模式3)
extern uint8_t NextMode; // 下一个模式 (1: 循迹, 2: 遥控, 3: 避障...)
extern uint8_t KeyNum;   // 按键编号

/* ==================== 状态标志 ==================== */
extern uint8_t SystemReady;  // 系统就绪标志
extern uint8_t MotorEnabled; // 电机使能状态 (0:禁用, 1:使能)

/* ==================== 函数声明 ==================== */
void Global_Init(void);
void Global_Loop(void);
void Global_Tick(void);

#endif
