
///////////////////////////////////////////////////////////////////////////
/// @file  xxx.h
/// @brief �������ͷ�ļ�
/// 
/// 
/// Ҫʹ���������,����밴�����²��������
/// 
/// 1) ��
/// 
/// 2) ;
///    
/// 2) ;
/// 3) ;
///////////////////////////////////////////////////////////////////////////

#ifndef _RS485_H_
#define _RS485_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define ShortInterval			    3		//�̰� ����ʱ�����
#define LongInterval			    35		//���� ����ʱ�����
#define DoubleHitInterval	        10		//��ֹ����˫�� �� ����
#define MAX_ADDR                    32
#define MIN_ADDR                    1

//extern void UART_Port_Init(uint32_t baudrate);
extern void RS485_Send(LPC_USART_T *pUART,uint8_t *Buffer, uint32_t Length);
extern void RS485_Init(void); 
extern uint8_t RS485_RX_Correct_flag;
extern void MessSend(void);
extern uint8_t  RS485_TX_buff[515];
extern uint8_t  RS485_Addr;
extern uint8_t baud;
extern uint8_t RS485_Store_Data[30];
extern uint16_t RS485_DOG;
extern uint8_t  RS485_RX_Count;
extern uint16_t RS485_CRC16_RES;

#ifdef __cplusplus
}
#endif

#endif  /* _XXX_H_ */
