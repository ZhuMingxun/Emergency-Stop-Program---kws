#include "chip.h"
#include "timer_cap.h"



uint16_t pinlv[20];  


/*********************************************************************************************************
** Function name:       void Timer_Cap_Init
** Descriptions:        定时器初始化  ，具体初始化哪个定时器，根据timer_cap.h是否“#define”决定
**						没有形参，预分频值固定为SystemCoreClock/10000，即每隔100us，TC+1，修改预分频值则需要同步修改
**						相应捕获中断计算频率的算法。
**						捕获方式统一为“下降沿捕获”、中断使能
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

/**************************************************************************************

功能：定时器初始化  ，具体初始化哪个定时器，根据timer_cap.h是否“#define”决定

没有形参，预分频值固定为SystemCoreClock/10000，即每隔100us，TC+1，修改预分频值则需要同步修改

相应捕获中断计算频率的算法

捕获方式统一为“下降沿捕获”、中断使能
**************************************************************************************/
void Timer_Cap_Init( )
{
	
	#ifdef TIMER16_0_CAP
		/* Enable timer clock */
		Chip_TIMER_Init(LPC_TIMER16_0);
	
		Chip_TIMER_Reset(LPC_TIMER16_0);    //禁止定时器，TC复位为1
	
		Chip_TIMER_PrescaleSet(LPC_TIMER16_0,SystemCoreClock/10000);   //100us TC+1
	
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, IOCON_FUNC2);	/* P0.2 TI16B0 CAP0 */
		
		Chip_TIMER_ClearCapture(LPC_TIMER16_0, 0);					
		/*CCR |= (0) * 3) + 1),CR0下降沿捕获*/
		Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER16_0, 0);		
		/*CCR |= ((0) * 3) + 2),CR0中断使能*/	
		Chip_TIMER_CaptureEnableInt(LPC_TIMER16_0, 0);				

		Chip_TIMER_Enable(LPC_TIMER16_0);

		NVIC_EnableIRQ(TIMER_16_0_IRQn);							// 使能CT16B0中断
	#endif
		
	#ifdef TIMER16_1_CAP
		/* Enable timer clock */
		Chip_TIMER_Init(LPC_TIMER16_1);
	
		Chip_TIMER_Reset(LPC_TIMER16_1);    //禁止定时器，TC复位为1
	
		Chip_TIMER_PrescaleSet(LPC_TIMER16_1,SystemCoreClock/10000);   //100us TC+1
	
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, IOCON_FUNC1);	/* P1.8 TI16B1 CAP0 */
		
		Chip_TIMER_ClearCapture(LPC_TIMER16_1, 0);					
		/*CCR |= (0) * 3) + 1),CR0下降沿捕获*/
		Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER16_1, 0);		
		/*CCR |= ((0) * 3) + 2),CR0中断使能*/	
		Chip_TIMER_CaptureEnableInt(LPC_TIMER16_1, 0);				

		Chip_TIMER_Enable(LPC_TIMER16_1);

		NVIC_EnableIRQ(TIMER_16_1_IRQn);							// 使能CT16B0中断
	#endif
	
	#ifdef TIMER32_0_CAP
		/* Enable timer clock */
		Chip_TIMER_Init(LPC_TIMER32_0);
	
		Chip_TIMER_Reset(LPC_TIMER32_0);    //禁止定时器，TC复位为1
	
		Chip_TIMER_PrescaleSet(LPC_TIMER32_0,SystemCoreClock/10000);   //100us TC+1
	
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, IOCON_FUNC2);	/* P1.5 TI32B0 CAP0 */
		
		Chip_TIMER_ClearCapture(LPC_TIMER32_0, 0);					
		/*CCR |= (0) * 3) + 1),CR0下降沿捕获*/
		Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER32_0, 0);		
		/*CCR |= ((0) * 3) + 2),CR0中断使能*/	
		Chip_TIMER_CaptureEnableInt(LPC_TIMER32_0, 0);				

		Chip_TIMER_Enable(LPC_TIMER32_0);

		NVIC_EnableIRQ(TIMER_32_0_IRQn);							// 使能CT16B0中断
	#endif
		
	#ifdef TIMER32_1_CAP
		/* Enable timer clock */
		Chip_TIMER_Init(LPC_TIMER32_1);
	
		Chip_TIMER_Reset(LPC_TIMER32_1);    //禁止定时器，TC复位为1
	
		Chip_TIMER_PrescaleSet(LPC_TIMER32_1,SystemCoreClock/10000);   //100us TC+1
	
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, IOCON_FUNC3);	/* P1.0 TI32B1 CAP0 */
		
		Chip_TIMER_ClearCapture(LPC_TIMER32_1, 0);					
		/*CCR |= (0) * 3) + 1),CR0下降沿捕获*/
		Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER32_1, 0);		
		/*CCR |= ((0) * 3) + 2),CR0中断使能*/	
		Chip_TIMER_CaptureEnableInt(LPC_TIMER32_1, 0);				

		Chip_TIMER_Enable(LPC_TIMER32_1);

		NVIC_EnableIRQ(TIMER_32_1_IRQn);							// 使能CT16B0中断
	#endif
	
	
}
/*采集完成后，丢弃第一个数据*/
#ifdef TIMER16_0_CAP
void TIMER16_0_IRQHandler(void)
{
	static uint8_t i = 0;
	uint16_t temp,freq;   
	if (Chip_TIMER_CapturePending(LPC_TIMER16_0, 0))
	{
		temp = Chip_TIMER_ReadCapture(LPC_TIMER16_0, 0);
		LPC_TIMER16_0->TC = 0;
		freq = 10000/temp; // 把单位转换成赫兹
		pinlv[i] = freq;
		i++;
		if(i == NumAcquisitions)
		{
			i = 0;
		}
	}
	Chip_TIMER_ClearCapture(LPC_TIMER16_0, 0);
}
#endif


/*采集完成后，丢弃第一个数据*/
#ifdef TIMER16_1_CAP
void TIMER16_1_IRQHandler(void)
{
	static uint8_t i = 0;
	uint16_t temp,freq;
	if (Chip_TIMER_CapturePending(LPC_TIMER16_1, 0))
	{
		temp = Chip_TIMER_ReadCapture(LPC_TIMER16_1, 0);
		LPC_TIMER16_1->TC = 0;
		freq = 10000/temp; // 把单位转换成赫兹
		pinlv[i] = freq;
		i++;
		if(i == NumAcquisitions)
		{
			i = 0;
		}
	}
	Chip_TIMER_ClearCapture(LPC_TIMER16_1, 0);
}
#endif

/*采集完成后，丢弃第一个数据*/
#ifdef TIMER32_0_CAP
void TIMER32_0_IRQHandler(void)
{
	static uint8_t i = 0;
	uint16_t temp,freq;
	if (Chip_TIMER_CapturePending(LPC_TIMER32_0, 0))
	{
		temp = Chip_TIMER_ReadCapture(LPC_TIMER32_0, 0);
		LPC_TIMER32_0->TC = 0;
		freq = 10000/temp; // 把单位转换成赫兹
		pinlv[i] = freq;
		i++;
		if(i == NumAcquisitions)
		{
			i = 0;
		}
	}
	Chip_TIMER_ClearCapture(LPC_TIMER32_0, 0);
}
#endif

/*采集完成后，丢弃第一个数据*/
#ifdef TIMER32_1_CAP
void TIMER32_1_IRQHandler(void)
{
	static uint8_t i = 0;
	uint16_t temp,freq;
	if (Chip_TIMER_CapturePending(LPC_TIMER32_1, 0))
	{
		temp = Chip_TIMER_ReadCapture(LPC_TIMER32_1, 0);
		LPC_TIMER32_1->TC = 0;
		freq = 10000/temp; // 把单位转换成赫兹
		pinlv[i] = freq;
		i++;
		if(i == NumAcquisitions)
		{
			i = 0;
		}
	}
	Chip_TIMER_ClearCapture(LPC_TIMER32_1, 0);
}
#endif
