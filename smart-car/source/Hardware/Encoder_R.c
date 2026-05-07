#include "stm32f10x.h" // Device header

int16_t Encoder_R_Count;            // 全局变量，用于计数旋转编码器的增量值
static uint8_t Encoder_R_State = 0; // 保存上一次编码器状态

// 正交编码器状态解码表
static const int8_t Encoder_Table[16] = {
    0, -1, +1, 0,
    +1, 0, 0, -1,
    -1, 0, 0, +1,
    0, +1, -1, 0};

/**
 * 函    数：旋转编码器初始化
 * 参    数：无
 * 返 回 值：无
 */
void Encoder_R_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*AFIO选择中断引脚*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);

    /*EXTI初始化*/
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = EXTI_Line13 | EXTI_Line14;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 双边沿触发
    EXTI_Init(&EXTI_InitStructure);

    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_Init(&NVIC_InitStructure);

    // 初始化编码器状态
    uint8_t a       = (GPIOB->IDR & GPIO_Pin_13) ? 1U : 0U;
    uint8_t b       = (GPIOB->IDR & GPIO_Pin_14) ? 1U : 0U;
    Encoder_R_State = (a << 1) | b;
}

/**
 * 函    数：旋转编码器获取增量值
 * 参    数：无
 * 返 回 值：自上此调用此函数后，旋转编码器的增量值
 */
int16_t Encoder_R_Get(void)
{
    int16_t Temp, Temp2;
    Temp            = Encoder_R_Count;
    Encoder_R_Count = 0;
    Temp2           = 199 * Temp / 43;
    return Temp2;
}

/**
 * 函    数：读取编码器当前状态
 * 参    数：无
 * 返 回 值：2位状态码 (A<<1 | B)
 */
static uint8_t Encoder_R_ReadState(void)
{
    uint8_t a = (GPIOB->IDR & GPIO_Pin_13) ? 1U : 0U;
    uint8_t b = (GPIOB->IDR & GPIO_Pin_14) ? 1U : 0U;
    return (a << 1) | b;
}

/**
 * 函    数：EXTI15_10外部中断函数
 * 参    数：无
 * 返 回 值：无
 */
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) == SET) {
        EXTI_ClearITPendingBit(EXTI_Line13);

        uint8_t curr = Encoder_R_ReadState();
        int8_t delta = Encoder_Table[(Encoder_R_State << 2) | curr];
        Encoder_R_Count += delta;
        Encoder_R_State = curr;
    }

    if (EXTI_GetITStatus(EXTI_Line14) == SET) {
        EXTI_ClearITPendingBit(EXTI_Line14);

        uint8_t curr = Encoder_R_ReadState();
        int8_t delta = Encoder_Table[(Encoder_R_State << 2) | curr];
        Encoder_R_Count += delta;
        Encoder_R_State = curr;
    }
}