#include "chip.h"
#include "timer_count.h"
uint16_t  section_count;

/*********************************************************************************************************
** Function name:       Timer_Count_Init( )
** Descriptions:        ��������ʼ��  �������ʼ���ĸ���ʱ��������timer_count.h�Ƿ�#define������

**						����ʽͳһΪ���½��ز��񡱡��ж�ʹ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/

/**************************************************************************************

���ܣ���ʱ����ʼ��  �������ʼ���ĸ���ʱ��������timer_count.h�Ƿ�#define������

**************************************************************************************/
void Timer_Count_Init( )
{
	
	#ifdef TIMER16_0_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, IOCON_FUNC2); /* P0.2 TI16B0 CAP0 */
		Chip_TIMER_Init(LPC_TIMER16_0);		/* Enable timer clock */
		Chip_TIMER_Reset(LPC_TIMER16_0);    //��ֹ��ʱ����TC��λΪ1
	
		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER16_0,TIMER_CAPSRC_RISING_CAPN,0)	;	//������ģʽ�������أ������ؼ���
		Chip_TIMER_SetMatch(LPC_TIMER16_0, TIMER16_0_COUNT_MATCH_NUM, TIMER16_0_COUNT_MATCH_VALUE);	// ƥ��Ĵ�������
		Chip_TIMER_MatchEnableInt(LPC_TIMER16_0, TIMER16_0_COUNT_MATCH_NUM);			//ʹ���ж�
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_0, TIMER16_0_COUNT_MATCH_NUM);	//ʹ��TC��λ

		Chip_TIMER_Enable(LPC_TIMER16_0);
		NVIC_EnableIRQ(TIMER_16_0_IRQn);							// ʹ��CT16B0�ж�
	#endif
		
	#ifdef TIMER16_1_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, IOCON_FUNC1); /* P1.8 TI16B1 CAP0 */
		Chip_TIMER_Init(LPC_TIMER16_1);	/* Enable timer clock */
		Chip_TIMER_Reset(LPC_TIMER16_1);    //��ֹ��ʱ����TC��λΪ1	
		
		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER16_1,TIMER_CAPSRC_RISING_CAPN,0);		//������ģʽ�������أ������ؼ���
		Chip_TIMER_SetMatch(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM, TIMER16_1_COUNT_MATCH_VALUE);	// ƥ��Ĵ�������
		Chip_TIMER_MatchEnableInt(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM);			//ʹ���ж�
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM);	//ʹ��TC��λ
		
		Chip_TIMER_Enable(LPC_TIMER16_1);
		NVIC_EnableIRQ(TIMER_16_1_IRQn);							// ʹ��CT16B1�ж�
	#endif
	
	#ifdef TIMER32_0_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_5, IOCON_FUNC2);	/* P1.5 TI32B0 CAP0 */
		Chip_TIMER_Init(LPC_TIMER32_0);		/* Enable timer clock */
		Chip_TIMER_Reset(LPC_TIMER32_0);	//��ֹ��ʱ����TC��λΪ1
			
		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER32_0,TIMER_CAPSRC_RISING_CAPN,0) ;	//������ģʽ�������أ������ؼ���
		Chip_TIMER_SetMatch(LPC_TIMER32_0, TIMER32_0_COUNT_MATCH_NUM, TIMER32_0_COUNT_MATCH_VALUE);	// ƥ��Ĵ�������
		Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, TIMER32_0_COUNT_MATCH_NUM);			//ʹ���ж�
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, TIMER32_0_COUNT_MATCH_NUM);	//ʹ��TC��λ

		Chip_TIMER_Enable(LPC_TIMER32_0);
		NVIC_EnableIRQ(TIMER_32_0_IRQn);							// ʹ��CT16B0�ж�
	#endif
		
	#ifdef TIMER32_1_COUNT
		Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, IOCON_FUNC3); /* P1.0 TI32B1 CAP0 */
		Chip_TIMER_Init(LPC_TIMER32_1);/* Enable timer clock */	
		Chip_TIMER_Reset(LPC_TIMER32_1);    //��ֹ��ʱ����TC��λΪ1

		Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER32_1,TIMER_CAPSRC_RISING_CAPN,0) ;	//������ģʽ�������أ������ؼ���
		Chip_TIMER_SetMatch(LPC_TIMER32_1, TIMER32_1_COUNT_MATCH_NUM, TIMER32_1_COUNT_MATCH_VALUE);	// ƥ��Ĵ�������
		Chip_TIMER_MatchEnableInt(LPC_TIMER32_1, TIMER32_1_COUNT_MATCH_NUM);			//ʹ���ж�
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_1, TIMER32_1_COUNT_MATCH_NUM);	//ʹ��TC��λ	

		Chip_TIMER_Enable(LPC_TIMER32_1);
		NVIC_EnableIRQ(TIMER_32_1_IRQn);							// ʹ��CT16B0�ж�
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
	if(Chip_TIMER_MatchPending(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM))		//ƥ���ж�
	{
		section_count++;
		Chip_TIMER_ClearMatch(LPC_TIMER16_1, TIMER16_1_COUNT_MATCH_NUM);		//���жϱ�־
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

