#include "soft_timer.h"
#include "message.h"
#include "system.h"


/// �����ʱ��״̬��ÿһλ��Ӧһ����ʱ����"1"������ע�ᣬ"0"�����ID��Ӧ�������ʱ����δ��ע��
static volatile uint32_t s_softTimerState; 

static volatile SOFT_TIM_TypeDef s_softTimerArray[SOFT_TIMER_NUM];


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	��ʼ�������ʱ��
///
/// @param[in]  ��
/// @reture     ��
//////////////////////////////////////////////////////////////////////////////////
void InitSoftTimers (void)
{
	s_softTimerState = 0;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	ע������һ���µ������ʱ��
///
/// @param[in]  mode   ��ʱʱ�䵽ʱ�Ĵ���ģʽ:���жϴ���or������Ϣ����ѭ������?
/// @param[in]  delay  ��ʱʱ��
/// @param[in]  callBackFunc   �ص�����
/// @reture     �����ʱ��ID,-1��ʾע��ʧ�ܣ������ʱ����������
///
//////////////////////////////////////////////////////////////////////////////////
int8_t StartSoftTimer(uint8_t mode, uint16_t delay, PFV callBackFunc)
{
	uint8_t i;
	EnterCritical();
	
	for(i = 0; i < SOFT_TIMER_NUM; i++)/*��ѯ�����������ʱ��*/
	{

		if(!GETBIT(s_softTimerState, i))	
		{			
			s_softTimerArray[i].delay        = delay;       /*��ʱʱ��*/
			s_softTimerArray[i].handleMode   = mode;        /*����ģʽ*/
			s_softTimerArray[i].callBackFunc = callBackFunc;/*�ص�����*/
						
			SETBIT(s_softTimerState, i);
			ExitCritical();
			return(i);
		}
	}
	
	ExitCritical();
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	ֹͣһ�������ʱ��
///
/// @param[in]  timerID   �����ʱ��ID
/// @reture     �ɹ�����TRUE,ʧ�ܷ���FALSE
///
//////////////////////////////////////////////////////////////////////////////////
uint8_t StopSoftTimer(uint8_t timerID)
{
	if(timerID < SOFT_TIMER_NUM)
	{
		EnterCritical();
		CLRBIT(s_softTimerState, timerID);
		ExitCritical();
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief �����ʱ��������ϵͳʱ���жϷ����е���
///
/// @param[in]  ��
/// @reture     ��
//////////////////////////////////////////////////////////////////////////////////
void SoftTimerService(void)
{
	uint8_t i = 0;
	uint32_t softTimerStateMap;

	EnterCritical();
	
	softTimerStateMap = s_softTimerState;
	while(softTimerStateMap)
	{
		if((softTimerStateMap & 0x01) == 1)
		{
			if(!(--s_softTimerArray[i].delay))/*����ʱ*/
			{
				if(s_softTimerArray[i].handleMode == TIMER_HANDLE_MODE_MSG)
				{
					/*�׳���Ϣ������ѭ����ִ��*/
					PostMessage(MSG_TYPE_SOFT_TIMER, MSG_PRIORITY_LOW, (uint32_t)s_softTimerArray[i].callBackFunc);
				}
				else
				{
					/*ֱ��ִ��*/
					Function(s_softTimerArray[i].callBackFunc); //����Ҫ����ʱ��ģ������ռ����Ӧʱ��
				}
				CLRBIT(s_softTimerState, i);
			}
		}
		
		softTimerStateMap = softTimerStateMap >> 1;
		i++;
	}
	ExitCritical();
}
