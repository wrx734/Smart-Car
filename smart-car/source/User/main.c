#include "stm32f10x.h" // Device header
#include "Mode1.h"
#include "Mode2.h"
#include "Mode3.h"
#include "Global.h"

int main(void)
{
    Global_Init();

    while (1) {
        Global_Loop();

        if (CurrMode == NextMode) {
            switch (CurrMode) {
                case 1:
                    Mode1_Loop();
                    break;
                case 2:
                    Mode2_Loop();
                    break;
                case 3:
                    Mode3_Loop();
                    break;
            }
        } else {
            switch (CurrMode) {
                case 1:
                    Mode1_Exit();
                    break;
                case 2:
                    Mode2_Exit();
                    break;
                case 3:
                    Mode3_Exit();
                    break;
            }
            switch (NextMode) {
                case 1:
                    Mode1_Init();
                    break;
                case 2:
                    Mode2_Init();
                    break;
                case 3:
                    Mode3_Init();
                    break;
            }
            CurrMode = NextMode;
        }
    }
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
        Global_Tick();

        if (CurrMode == NextMode) {
            switch (CurrMode) {
                case 1:
                    Mode1_Tick();
                    break;
                case 2:
                    Mode2_Tick();
                    break;
                case 3:
                    Mode3_Tick();
                    break;
            }
        }

        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}
