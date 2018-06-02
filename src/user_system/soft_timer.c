#include "soft_timer.h"
#include "message.h"
#include "system.h"


/// 软件定时器状态，每一位对应一个定时器，"1"代表已注册，"0"代表该ID对应的软件定时器还未被注册
static volatile uint32_t s_softTimerState; 

static volatile SOFT_TIM_TypeDef s_softTimerArray[SOFT_TIMER_NUM];


//////////////////////////////////////////////////////////////////////////////////
/// @brief 	初始化软件定时器
///
/// @param[in]  无
/// @reture     无
//////////////////////////////////////////////////////////////////////////////////
void InitSoftTimers (void)
{
	s_softTimerState = 0;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	注册启动一个新的软件定时器
///
/// @param[in]  mode   延时时间到时的处理模式:在中断处理or发送消息到主循环处理?
/// @param[in]  delay  延时时间
/// @param[in]  callBackFunc   回调函数
/// @reture     软件定时器ID,-1表示注册失败，软件定时器数组已满
///
//////////////////////////////////////////////////////////////////////////////////
int8_t StartSoftTimer(uint8_t mode, uint16_t delay, PFV callBackFunc)
{
	uint8_t i;
	EnterCritical();
	
	for(i = 0; i < SOFT_TIMER_NUM; i++)/*查询并分配软件定时器*/
	{

		if(!GETBIT(s_softTimerState, i))	
		{			
			s_softTimerArray[i].delay        = delay;       /*延时时间*/
			s_softTimerArray[i].handleMode   = mode;        /*处理模式*/
			s_softTimerArray[i].callBackFunc = callBackFunc;/*回调函数*/
						
			SETBIT(s_softTimerState, i);
			ExitCritical();
			return(i);
		}
	}
	
	ExitCritical();
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	停止一个软件定时器
///
/// @param[in]  timerID   软件定时器ID
/// @reture     成功返回TRUE,失败返回FALSE
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
/// @brief 软件定时器服务，在系统时钟中断服务中调用
///
/// @param[in]  无
/// @reture     无
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
			if(!(--s_softTimerArray[i].delay))/*倒计时*/
			{
				if(s_softTimerArray[i].handleMode == TIMER_HANDLE_MODE_MSG)
				{
					/*抛出消息，在主循环中执行*/
					PostMessage(MSG_TYPE_SOFT_TIMER, MSG_PRIORITY_LOW, (uint32_t)s_softTimerArray[i].callBackFunc);
				}
				else
				{
					/*直接执行*/
					Function(s_softTimerArray[i].callBackFunc); //必须要极短时间的，否则会占用响应时间
				}
				CLRBIT(s_softTimerState, i);
			}
		}
		
		softTimerStateMap = softTimerStateMap >> 1;
		i++;
	}
	ExitCritical();
}
