#include "stm32f10x.h" // Device header
#include <stdio.h>
#include <stdarg.h>

#define SERIAL_RX_TIMEOUT_MS 20		// 接收超时时间，单位：毫秒

// char Serial_RxPacket[100];
uint8_t Serial_RxPacket[4];
volatile int16_t x;
volatile int16_t y;
volatile uint8_t Serial_RxFlag; // 定义接收数据包标志位

// 定义一些静态变量，用于串口接收数据包的状态管理
static uint8_t Serial_RxState = 0;		// 接收状态，0：等待接收数据包包头，1：接收数据包数据，2：接收数据包校验和
static uint8_t Serial_pRxPacket = 0;		// 接收数据包指针，指向当前接收的数据包
static uint16_t Serial_RxTimeoutCnt = 0;		// 接收超时计数，单位：毫秒

/**
 * 函    数：串口初始化
 * 参    数：无
 * 返 回 值：无
 */
void Serial_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // 开启USART3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 开启GPIOB的时钟

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PB10引脚初始化为复用推挽输出

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PB11引脚初始化为上拉输入

	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;											// 定义结构体变量
	USART_InitStructure.USART_BaudRate = 115200;									// 波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					// 模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长，选择8位
	USART_Init(USART3, &USART_InitStructure);										// 将结构体变量交给USART_Init，配置USART3

	/*中断输出配置*/
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置NVIC为分组2

	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					  // 定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		  // 选择配置NVIC的USART3线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // 指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							  // 将结构体变量交给NVIC_Init，配置NVIC外设

	/*USART使能*/
	USART_Cmd(USART3, ENABLE); // 使能USART3，串口开始运行
}

/**
 * 函    数：串口发送一个字节
 * 参    数：Byte 要发送的一个字节
 * 返 回 值：无
 */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte); // 将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		; // 等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
 * 函    数：串口发送一个数组
 * 参    数：Array 要发送数组的首地址
 * 参    数：Length 要发送数组的长度
 * 返 回 值：无
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i++) // 遍历数组
	{
		Serial_SendByte(Array[i]); // 依次调用Serial_SendByte发送每个字节数据
	}
}

/**
 * 函    数：串口发送一个字符串
 * 参    数：String 要发送字符串的首地址
 * 返 回 值：无
 */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++) // 遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Serial_SendByte(String[i]); // 依次调用Serial_SendByte发送每个字节数据
	}
}

/**
 * 函    数：次方函数（内部使用）
 * 返 回 值：返回值等于X的Y次方
 */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1; // 设置结果初值为1
	while (Y--)			 // 执行Y次
	{
		Result *= X; // 将X累乘到结果
	}
	return Result;
}

/**
 * 函    数：串口发送数字
 * 参    数：Number 要发送的数字，范围：0~4294967295
 * 参    数：Length 要发送数字的长度，范围：0~10
 * 返 回 值：无
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++) // 根据数字长度遍历数字的每一位
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0'); // 依次调用Serial_SendByte发送每位数字
	}
}

/**
 * 函    数：使用printf需要重定向的底层函数
 * 参    数：保持原始格式即可，无需变动
 * 返 回 值：保持原始格式即可，无需变动
 */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch); // 将printf的底层重定向到自己的发送字节函数
	return ch;
}

/**
 * 函    数：自己封装的prinf函数
 * 参    数：format 格式化字符串
 * 参    数：... 可变的参数列表
 * 返 回 值：无
 */
void Serial_Printf(char *format, ...)
{
	char String[100];			   // 定义字符数组
	va_list arg;				   // 定义可变参数列表数据类型的变量arg
	va_start(arg, format);		   // 从format开始，接收参数列表到arg变量
	vsnprintf(String, sizeof(String), format, arg); // 限制输出长度，避免缓冲区溢出
	va_end(arg);				   // 结束变量arg
	Serial_SendString(String);	   // 串口发送字符数组（字符串）
}

// 接收超时函数，每1ms调用一次
void Serial_RxTimeoutTick(void)	
{
	if (Serial_RxState != 0)	// 如果当前状态不是接收数据包包头状态
	{
		Serial_RxTimeoutCnt++;	// 接收超时计数自增
		if (Serial_RxTimeoutCnt >= SERIAL_RX_TIMEOUT_MS)		// 如果接收超时计数达到设定的超时时间
		{
			Serial_RxState = 0;			// 将状态设为0 等待接收数据包包头状态
			Serial_pRxPacket = 0;		// 数据包的位置归零
			Serial_RxTimeoutCnt = 0;	// 接收超时计数归零
		}
	}
}

// 获取当前接收的x和y值的快照，用于在主循环中计算和显示
void Serial_GetXYSnapshot(int16_t *x_out, int16_t *y_out)	
{
	__disable_irq();
	*x_out = x;		// 将当前接收的x值赋值给x_out指针指向的内存地址
	*y_out = y;		// 将当前接收的y值赋值给y_out指针指向的内存地址
	__enable_irq();
}

/**
 * 函    数：USART3中断函数
 * 参    数：无
 * 返 回 值：无
 * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
 *           函数名为预留的指定名称，可以从启动文件复制
 *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
 */
void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) // 判断是否是USART3的接收事件触发的中断
	{
		uint8_t RxData = USART_ReceiveData(USART3); // 读取数据寄存器，存放在接收的数据变量
		Serial_RxTimeoutCnt = 0; // 接收超时计数归零

		/*使用状态机的思路，依次处理数据包的不同部分*/
		/*当前状态为0，接收数据包包头*/
		if (Serial_RxState == 0)
		{
			if (RxData == 0xAA) // 如果数据确实是包头
			{
				Serial_RxState = 1; // 置下一个状态
			}
		}
		/*当前状态为1，接收第二个包头*/
		else if (Serial_RxState == 1)
		{
			if (RxData == 0xAA) // 如果数据确实是包头
			{
				Serial_RxState = 2;	  // 置下一个状态
				Serial_pRxPacket = 0; // 数据包的位置归零
			}
			else
			{
				Serial_RxState = 0;	  // 置下一个状态
			}
		}
		else if (Serial_RxState == 2)
		{
			Serial_RxPacket[Serial_pRxPacket] = RxData; // 将数据存入数据包数组的指定位置
			Serial_pRxPacket++;						 // 数据包的位置自增
			if (Serial_pRxPacket >= 4)					 // 如果收够4个数据
			{
				Serial_RxState = 3; // 置下一个状态
			}
		}
		else if (Serial_RxState == 3)
		{
			if (RxData == 0x55) // 如果数据确实是包尾部
			{
				Serial_RxState = 4; // 置下一个状态
			}
			else if (RxData == 0xAA)
			{
				Serial_RxState = 1;
			}
			else
			{
				Serial_RxState = 0;
			}
		}
		else if (Serial_RxState == 4)
		{
			if (RxData == 0x55) // 如果数据确实是包尾部
			{
				int16_t x_new = (int16_t)(((uint16_t)Serial_RxPacket[1] << 8) | Serial_RxPacket[0]);
					// 将数据包数组中的前两个字节合成一个16位有符号整数，存放在x_new变量中
				int16_t y_new = (int16_t)(((uint16_t)Serial_RxPacket[3] << 8) | Serial_RxPacket[2]);
					// 将数据包数组中的后两个字节合成一个16位有符号整数，存放在y_new变量中
				x = x_new; // 将x_new赋值给x变量
				y = y_new; // 将y_new赋值给y变量
				Serial_RxState = 0;	   // 置下一个状态
				Serial_pRxPacket = 0;	// 数据包的位置归零
				Serial_RxFlag = 1; // 接收数据包标志位置1，成功接收一个数据包
				Serial_RxTimeoutCnt = 0;	// 接收超时计数归零
			}
			else if (RxData == 0xAA)
			{
				Serial_RxState = 1;
				Serial_pRxPacket = 0;
			}
			else
			{
				Serial_RxState = 0;
				Serial_pRxPacket = 0;
			}
		}

		USART_ClearITPendingBit(USART3, USART_IT_RXNE); // 清除标志位
	}
}
