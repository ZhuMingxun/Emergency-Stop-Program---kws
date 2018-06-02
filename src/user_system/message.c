#include "message.h"
#include "system.h"

static volatile MSG_QUEQUE_TypeDef s_messageQueue;

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	��Ϣ���г�ʼ��
///
/// @param[in]  ��
/// @reture     ��
///
//////////////////////////////////////////////////////////////////////////////////
void InitMessageQueue (void)
{
	s_messageQueue.readIndex = 0;
	s_messageQueue.writeIndex = 0;
	s_messageQueue.numMsgInQueue = 0;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	��������Ϣ����Ϣ����
///
/// @param[in]  msgType  ��Ϣ����
/// @param[in]  msgPriority  ��Ϣ���ȼ�
/// @param[in]  msgValue  ��Ϣ����ֵ
/// @reture     �ɹ�����TRUE,ʧ�ܷ���FALSE(��������)
///
//////////////////////////////////////////////////////////////////////////////////
uint8_t  PostMessage (uint8_t msgType, uint8_t msgPriority, uint32_t msgValue)

{
	#ifdef PRIORITY_MESSAGE_QUEUE /// ���ȶ���
	uint8_t i, m, n;
	MSG_TypeDef tmpMsg;
	#endif
	
	EnterCritical();
	
	if (s_messageQueue.numMsgInQueue >= MESSAGE_QUEUE_SIZE)
	{
		ExitCritical();
		return FALSE;
	}
	
	s_messageQueue.messageArray[s_messageQueue.writeIndex].value = msgValue;
	s_messageQueue.messageArray[s_messageQueue.writeIndex].type  = msgType;
	s_messageQueue.messageArray[s_messageQueue.writeIndex].priority = msgPriority;
	if (++(s_messageQueue.writeIndex) >= MESSAGE_QUEUE_SIZE)
	{
		s_messageQueue.writeIndex = 0;
	}
	(s_messageQueue.numMsgInQueue)++;

	#ifdef PRIORITY_MESSAGE_QUEUE /// ���ȶ���
	if (s_messageQueue.numMsgInQueue > 1)
	{
		if(s_messageQueue.readIndex < s_messageQueue.writeIndex)
		{
			i = s_messageQueue.writeIndex - 1;
		}
		else
		{
			i = MESSAGE_QUEUE_SIZE + s_messageQueue.writeIndex - 1;
		}

		while (i > s_messageQueue.readIndex)
		{
			(i>=MESSAGE_QUEUE_SIZE)?(m=i-MESSAGE_QUEUE_SIZE):(m=i);
			(i>=MESSAGE_QUEUE_SIZE+1)?(n=i-1-MESSAGE_QUEUE_SIZE):(n=i-1);
			if (s_messageQueue.messageArray[m].priority <= s_messageQueue.messageArray[n].priority)
			{	
				break;
			}
			tmpMsg = s_messageQueue.messageArray[m];
			s_messageQueue.messageArray[m] = s_messageQueue.messageArray[n];
			s_messageQueue.messageArray[n] = tmpMsg;
			i--;
		}
	}
	#endif

	ExitCritical();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief 	����Ϣ���ж�ȡ��Ϣ
///
/// @param[out]  pNewMsg   ָ������Ϣ��ָ��
/// @reture     �ɹ�����TRUE,ʧ�ܷ���FALSE(����Ϊ��)
///
//////////////////////////////////////////////////////////////////////////////////
uint8_t PendMessage (P_MSG_TypeDef pNewMsg)
{
	if (s_messageQueue.numMsgInQueue == 0)
	{
		return FALSE;
	}
	EnterCritical();

	*pNewMsg = s_messageQueue.messageArray[s_messageQueue.readIndex];
	//pNewMsg->type  = s_messageQueue.messageArray[s_messageQueue.readIndex].type;
	//pNewMsg->value = s_messageQueue.messageArray[s_messageQueue.readIndex].value;
	//pNewMsg->priority = s_messageQueue.messageArray[s_messageQueue.readIndex].priority;
	if (++(s_messageQueue.readIndex) >= MESSAGE_QUEUE_SIZE)
	{
		s_messageQueue.readIndex = 0;
	}
	s_messageQueue.numMsgInQueue--;

	ExitCritical();
	return TRUE;

}
