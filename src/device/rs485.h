
///////////////////////////////////////////////////////////////////////////
/// @file  xxx.h
/// @brief 组件程序头文件
/// 
/// 
/// 要使用这个驱动,你必须按照如下步骤操作：
/// 
/// 1) ：
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

#define ShortInterval			    3		//短按 按键时间节拍
#define LongInterval			    35		//长按 按键时间节拍
#define DoubleHitInterval	        10		//防止连续双击 误动 节拍
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
