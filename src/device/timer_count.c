#include "chip.h"
#include "timer_count.h"
uint16_t  section_count;

/*********************************************************************************************************
** Function name:       Timer_Count_Init( )
** Descriptions:        计数器初始化  ，具体初始化哪个定时器，根据timer_count.h是否“#define”决定

**						捕获方式统一为“下降沿捕获”、中断使能
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/

/**************************************************************************************

功能：定时器初始化  ，具体初始化哪个定时器，根据timer_count.h是否“#define”决定

**************************************************************************************/
void Timer_Count_Init( )
{
	
	#ifdef TIMER16_0_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, IOCON_FUNC2); /* P0.2 TI16B0 CAP0 */
		Chip_TIMER_Init(LPC_TIMER16_0);		/* Enable timer clock */
		Chip_TIMER_Reset(LPC_TIMER16_0);    //禁止定时器，TC复位为1
	
		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER16_0,TIMER_CAPSRC_RISING_CAPN,0)	;	//计数器模式，单边沿，上升沿计数
		Chip_TIMER_SetMatch(LPC_TIMER16_0, TIMER16_0_COUNT_MATCH_NUM, TIMER16_0_COUNT_MATCH_VALUE);	// 匹配寄存器设置
		Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, TIMER16_0_COUNT_MATCH_NUM);			//使能中断
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_0, TIMER16_0_COUNT_MATCH_NUM);	//使能TC复位

		Chip_TIMER_Enable(LPC_TIMER16_0);
		NVIC_EnableIRQ(TIMER_16_0_IRQn);							// 使能CT16B0中断
	#endif
		
	#ifdef TIMER16_1_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, IOCON_FUNC1); /* P1.8 TI16B1 CAP0 */
		Chip_TIMER_Init(LPC_TIMER16_1);	/* Enable timer clock */
		Chip_TIMER_Reset(LPC_TIMER16_1);    //禁止定时器，TC复位为1	
		
		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER16_1,TIMER_CAPSRC_RISING_CAPN,0);		//计数器模式，单边沿，上升沿计数
		Chip_TIMER_SetMatch(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM, TIMER16_1_COUNT_MATCH_VALUE);	// 匹配寄存器设置
		Chip_TIMER_MatchEnableInt(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM);			//使能中断
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM);	//使能TC复位
		
		Chip_TIMER_Enable(LPC_TIMER16_1);
		NVIC_EnableIRQ(TIMER_16_1_IRQn);							// 使能CT16B1中断
	#endif
	
	#ifdef TIMER32_0_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, IOCON_FUNC2);	/* P1.5 TI32B0 CAP0 */
		Chip_TIMER_Init(LPC_TIMER32_0);		/* Enable timer clock */
		Chip_TIMER_Reset(LPC_TIMER32_0);	//禁止定时器，TC复位为1
			
		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER32_0,TIMER_CAPSRC_RISING_CAPN,0) ;	//计数器模式，单边沿，上升沿计数
		Chip_TIMER_SetMatch(LPC_TIMER32_0, TIMER32_0_COUNT_MATCH_NUM, TIMER32_0_COUNT_MATCH_VALUE);	// 匹配寄存器设置
		Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, TIMER32_0_COUNT_MATCH_NUM);			//使能中断
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, TIMER32_0_COUNT_MATCH_NUM);	//使能TC复位

		Chip_TIMER_Enable(LPC_TIMER32_0);
		NVIC_EnableIRQ(TIMER_32_0_IRQn);							// 使能CT16B0中断
	#endif
		
	#ifdef TIMER32_1_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, IOCON_FUNC3); /* P1.0 TI32B1 CAP0 */
		Chip_TIMER_Init(LPC_TIMER32_1);/* Enable timer clock */	
		Chip_TIMER_Reset(LPC_TIMER32_1);    //禁止定时器，TC复位为1

		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER32_1,TIMER_CAPSRC_RISING_CAPN,0) ;	//计数器模式，单边沿，上升沿计数
		Chip_TIMER_SetMatch(LPC_TIMER32_1, TIMER32_1_COUNT_MATCH_NUM, TIMER32_1_COUNT_MATCH_VALUE);	// 匹配寄存器设置
		Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, TIMER32_1_COUNT_MATCH_NUM);			//使能中断
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_1, TIMER32_1_COUNT_MATCH_NUM);	//使能TC复位	

		Chip_TIMER_Enable(LPC_TIMER32_1);
		NVIC_EnableIRQ(TIMER_32_1_IRQn);							// 使能CT16B0中断
	#endif	
	
}


// /**/
// #ifdef TIMER16_0_COUNT
// void TIMER16_0_IRQHandler(void)
// {
// 	
// 	;
// }
// #endif


// /**/
#ifdef TIMER16_1_COUNT
void TIMER16_1_IRQHandler(void)
{
	if(Chip_TIMER_MatchPending(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM))		//匹配中断
	{
		section_count++;
		Chip_TIMER_ClearMatch(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM);		//清中断标志
	}	
}
#endif

/**/
#ifdef TIMER32_0_COUNT
void TIMER32_0_IRQHandler(void)
{
	;	

}
#endif

/**/
#ifdef TIMER32_1_COUNT
void TIMER32_1_IRQHandler(void)
{
	;	

}
#endif

