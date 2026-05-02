#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

extern uint8_t Serial_RxPacket[4];
extern volatile int16_t x;
extern volatile int16_t y;
extern volatile uint8_t Serial_RxFlag;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);
void Serial_RxTimeoutTick(void);
void Serial_GetXYSnapshot(int16_t *x_out, int16_t *y_out);

#endif
