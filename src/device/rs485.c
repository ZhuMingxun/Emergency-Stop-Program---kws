
///////////////////////////////////////////////////////////////////////////
/// @file  led.c
/// @brief ���Դ����
/// 
/// ���Դ����������ܣ������ж��вɼ�PIO3_3,ͨ����Ϣ���ݺ�������
/// ��message.h�ж�����Ϣ���ͣ�#define MSG_TYPE_XXX             2
/// <b>Copyright ��c�� 
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////
/// includes
/////////////////////////////////////////////////
#include "chip.h"
#include "rs485.h"
#include "Message.h"

/////////////////////////////////////////////////////////////////////
/// ��������
////////////////////////////////////////////////////////////////////
#define Read_Holding_Register      3	/*0x03������궨��*/
#define Preset_Single_Register     6	/*0x06������궨��*/
#define Multi_Single_Register      16	/*0x10������궨��*/

#define RS485_TX 	Chip_GPIO_SetPinState(LPC_GPIO, 1, 0, 0)
#define RS485_RX 	Chip_GPIO_SetPinState(LPC_GPIO, 1, 0, 1)

uint8_t  baud = 0;
uint8_t  RS485_Addr;					//�ֻ���ַ
uint8_t  RS485_TX_buff[515];		    //��ȷ���պ�ظ���������,���515���ֽڣ�������Ҫ��С���Ĵ�������0xff
uint8_t  RS485_RX_buff[264];			//���������ݴ����飬���264���ֽڣ��Ĵ�������0xff
uint8_t  RS485_TX_Length;               //�������ݵĳ���
uint8_t  RS485_FUN_flag;				//RS485�������־
uint8_t  RS485_START_AddrH;				//��ʼ��ַ���ֽ�
uint8_t  RS485_START_AddrL;				//��ʼ��ַ���ֽ�
uint8_t  RS485_Present_PDH;				//Ԥ�ø������ֽ�
uint8_t  RS485_Present_PDL;				//Ԥ�ø������ֽ�
uint8_t  RS485_RX_Correct_flag;			//������ȷ��־
uint16_t RS485_DOG;
uint8_t  RS485_RX_Count;
uint16_t RS485_CRC16_RES = 0XFFFF;

uint8_t  RS485_Store_Data[30]={0xff};
uint8_t count;
void Delayms()					
{
	uint16_t z;
	for(z = 8000;z > 0; z--);
}
/***************************************************
�������ƣ�UART_Port_Init
��ڲ�����baudrate:������
�������ܣ�UART��ʼ����9600,8N1
****************************************************/
void UART_Port_Init(uint32_t baudrate)
{
	
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));  /* P1.6 = RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));  /* P1.7 = TXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, (IOCON_FUNC1 | IOCON_MODE_INACT));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 7);	                              /*P1.7 = TXD,OUTPUT   */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO,  1, 6);								  /*P1.6 = RXD,INPUT    */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 0);	                              /*P1.0 = RSDIR,OUTPUT */
	
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, baudrate);
	Chip_UART_ConfigData(LPC_USART, ( UART_LCR_WLEN8|UART_LCR_SBS_1BIT ));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV0));

	Chip_UART_IntEnable(LPC_USART, UART_IER_RBRINT );
	NVIC_EnableIRQ(UART0_IRQn);

	RS485_RX;
	RS485_RX_Correct_flag = 0;
	RS485_Addr = 1;
}
/***************************************************
�������ƣ�RS485_CRC16
��ڲ�����rec_dat:��ǰ���յ������ݣ�crc16_result:ǰУ����
�������ܣ�ʵ��CRC16У�鹦��
****************************************************/
uint16_t RS485_CRC16(uint8_t rec_dat,uint16_t crc16_result)
{
	uint8_t i;
	uint16_t z;	 	
	crc16_result ^= rec_dat; 
	 
	for(i=0;i<8;i++)
	{	 
		if((crc16_result&1) == 1)
		{
			crc16_result >>= 1;
			crc16_result ^= 0xA001;	
		}
		else
		{
			crc16_result >>= 1;
		} 
	}
	z = crc16_result;
	return(z);
}
/***************************************************
�������ƣ�RS485_Send
��ڲ�����UART���������/���ݵ�ַ������
�������ܣ�UART��������
****************************************************/
void RS485_Send(LPC_USART_T *pUART,uint8_t *Buffer, uint32_t Length)
{
	while(Length != 0)
	{
		while ( !(Chip_UART_ReadLineStatus(pUART) & (1<<5)) );
		Chip_UART_SendByte(pUART, *Buffer);
		Buffer++;
		Length--;
	}
}
/***************************************************
�������ƣ�MessSend
��ڲ�������
�������ܣ�����װ��
****************************************************/
void MessSend()
{
	uint8_t  x;								   //ѭ������
	uint16_t Temp_Lth;						   //�ֽڳ��ȿ���
	uint16_t Addr_offset;					   //��ʼ��ַ
	uint16_t Temp_Con;						   //���ڴ�Ŷ�ȡ�ֽڸ���
	uint16_t RS485_TX_CRC16_RES = 0xffff;      //У��ֵ

	Chip_UART_IntDisable(LPC_USART, UART_IER_RBRINT);
	RS485_TX;								   //��Ϊ����״̬
	if(RS485_FUN_flag != 0)
	{
		switch(RS485_FUN_flag)
		{
////////////////�������Ĵ�/////////////////////////////////////////////////////////
			case Read_Holding_Register:			
		   	Temp_Lth = ( RS485_RX_buff[2]*256 + RS485_RX_buff[3])*2; //�����ȡ�ֽڸ���
			if(Temp_Lth > 254) Temp_Con = 254;					     //�Ĵ�����������
			else Temp_Con = Temp_Lth;
			Addr_offset = 2*(RS485_RX_buff[0]*256 + RS485_RX_buff[1]);	 //�����������ݵ���ʼ��ַ
			RS485_TX_buff[0] = RS485_Addr;						     //�ֻ���ַ
			RS485_TX_buff[1] = RS485_FUN_flag;					     //������
			RS485_TX_buff[2] = Temp_Con;						     //�ֽڸ���
			for(x = 0; x < Temp_Con; x++)						   
			{
				RS485_TX_buff[3 + x] = RS485_Store_Data[Addr_offset + x];                       //�˴�Ҫװ������
			}
			for(x = 0; x < 3+Temp_Con; x++)					         //У��ֵ
			{
				RS485_TX_CRC16_RES = RS485_CRC16(RS485_TX_buff[x],RS485_TX_CRC16_RES);	
			}
			RS485_TX_buff[3+Temp_Con] = RS485_TX_CRC16_RES;
			RS485_TX_buff[4+Temp_Con] = RS485_TX_CRC16_RES>>8;
			RS485_TX_Length = 5+Temp_Con;	                         //���ݳ���
			RS485_FUN_flag = 0;
			break;
////////////////////////////////////////////////////////////////////////////////////////////
/////////////////Ԥ�õ����Ĵ���///////////////////////////////////////////////////////////
			case Preset_Single_Register:
			Addr_offset =2*(RS485_RX_buff[0]*256 + RS485_RX_buff[1]);	 //������д���ݵ���ʼ��ַ							 
			RS485_TX_buff[0] = RS485_Addr;							 //װ�ص�ַ
			RS485_TX_buff[1] = RS485_FUN_flag;						 //װ�ع�����
			for(x = 0; x < 4; x++)
			{
				RS485_TX_buff[2+x] = RS485_RX_buff[x];				 //װ�ػظ�����
			}
			for(x = 0;x < 2; x++)
			{
				RS485_Store_Data[Addr_offset + x] =  RS485_RX_buff[2+x]; //�洢����
			}
			for(x = 0; x < 6; x++)
			{
				RS485_TX_CRC16_RES = RS485_CRC16(RS485_TX_buff[x],RS485_TX_CRC16_RES);	
			}
			RS485_TX_buff[6] = RS485_TX_CRC16_RES;
			RS485_TX_buff[7] = RS485_TX_CRC16_RES>>8;
			RS485_TX_Length = 8;
			RS485_FUN_flag = 0;
			break;
//////////////////////////Ԥ�ö���Ĵ���//////////////////////////////
			case  Multi_Single_Register:
			Addr_offset = RS485_RX_buff[0]*256 + RS485_RX_buff[1];	 //������д���ݵ���ʼ��ַ							 
			RS485_TX_buff[0] = RS485_Addr;							 //װ�ص�ַ
			RS485_TX_buff[1] = RS485_FUN_flag;						 //װ�ع�����
			for(x = 0; x < 4; x++)
			{
				RS485_TX_buff[2+x] = RS485_RX_buff[x];				 //װ�ػظ�����
			}
			for(x = 0;x < RS485_RX_buff[4]; x++)
			{
				RS485_Store_Data[Addr_offset + x] =  RS485_RX_buff[5+x]; //�洢����
			}
			for(x = 0; x < 6; x++)
			{
				RS485_TX_CRC16_RES = RS485_CRC16(RS485_TX_buff[x],RS485_TX_CRC16_RES);	
			}
			RS485_TX_buff[6] = RS485_TX_CRC16_RES;
			RS485_TX_buff[7] = RS485_TX_CRC16_RES>>8;
			RS485_TX_Length = 8;
			RS485_FUN_flag = 0;			

			break;
////////////////////////////////////////////////////////////////////////////////////////////
			default:
			break;
		}
	}
	Delayms();
	RS485_Send(LPC_USART,RS485_TX_buff,RS485_TX_Length);
	Delayms();
	Delayms();
	Chip_UART_IntEnable(LPC_USART,UART_IER_RBRINT);	
	RS485_RX;

}

/***************************************************
�������ƣ�RS485_Rev
��ڲ�����UART���
�������ܣ����ݽ���
****************************************************/
void RS485_Rev(LPC_USART_T *pUART)
{
	static uint16_t Check_RES;					  //У����
//	static uint8_t  RS485_RX_Count;				  //RS485�����ֽڼ�����
	static uint8_t  RS485_FUN_num;				  //RS485������
//	static uint16_t RS485_CRC16_RES = 0XFFFF;     //CRCУ���ʼ���
	uint8_t RS485_RX_Reg;

	while ((Chip_UART_ReadIntIDReg(pUART) & 0x01) == 0 ) 
	{
		if( (Chip_UART_ReadIntIDReg(pUART) & 0x0E) == 0x04 )
		{
			RS485_RX_Reg = 	Chip_UART_ReadByte(pUART);
			RS485_RX_Count++;
			
			RS485_DOG = 30;
			if(RS485_RX_Count == 1)				  //���յ���һ���ֽ�
			{
				if(RS485_RX_Reg == RS485_Addr)	  //��һ���ǵ�ַ����
				{
					RS485_CRC16_RES = RS485_CRC16(RS485_RX_Reg,RS485_CRC16_RES);	
				}
				else 
				{
					RS485_RX_Count = 0;	
					RS485_CRC16_RES = 0XFFFF;
				}
			}
			else if(RS485_RX_Count == 2)		 //�ڶ����ֽ��ǹ�����
			{
				switch(RS485_RX_Reg)
				{
					case  Read_Holding_Register:   
					case  Preset_Single_Register:
					case  Multi_Single_Register:
					RS485_CRC16_RES = RS485_CRC16(RS485_RX_Reg,RS485_CRC16_RES);
					RS485_FUN_num =  RS485_RX_Reg;

					break;
					default:
					RS485_RX_Count = 0;
					RS485_CRC16_RES = 0XFFFF;
				}
			}
			else if( (RS485_RX_Count == 3)||(RS485_RX_Count == 4) )
			{
				switch(RS485_FUN_num)
				{
					case  Read_Holding_Register:
					case  Preset_Single_Register:
					case  Multi_Single_Register:
					if(RS485_RX_Count == 3)
					{
						RS485_START_AddrH = RS485_RX_Reg ;		//�洢���ֽڵ�ַ
					}
					else
					{
					 	RS485_START_AddrL = RS485_RX_Reg ;		//�洢���ֽڵ�ַ
					}
					RS485_CRC16_RES = RS485_CRC16(RS485_RX_Reg,RS485_CRC16_RES);
					break;
					default:
					RS485_RX_Count = 0;
					RS485_CRC16_RES = 0XFFFF;	
					break;				
				}
			}
			else if( (RS485_RX_Count == 5)||(RS485_RX_Count == 6) )
			{
				switch(RS485_FUN_num)
				{
					case  Read_Holding_Register:
					case  Preset_Single_Register:
					case  Multi_Single_Register:
					if(RS485_RX_Count == 5)
					{
						RS485_Present_PDH = RS485_RX_Reg ;		//�洢���ֽڸ���
					}
					else
					{
					 	RS485_Present_PDL = RS485_RX_Reg ;		//�洢���ֽڸ���
					}
					RS485_CRC16_RES = RS485_CRC16(RS485_RX_Reg,RS485_CRC16_RES);
					break;
					default:
					RS485_RX_Count = 0;
					RS485_CRC16_RES = 0XFFFF;	
					break;				
				}				
			}
			else if( (RS485_RX_Count == 7)||(RS485_RX_Count == 8) )
			{
				switch(RS485_FUN_num)
				{
					case  Read_Holding_Register:
					case  Preset_Single_Register:
					if(RS485_RX_Count == 7)
					{
						Check_RES = RS485_RX_Reg ;		          //У��λ�ĵ��ֽ�
					}
					else
					{
					 	Check_RES += RS485_RX_Reg*256;            //У����
						if(Check_RES == RS485_CRC16_RES) 
						{
							RS485_FUN_flag = RS485_FUN_num;       //��ȡ������
							RS485_RX_buff[0] = RS485_START_AddrH; //��ʼ��ַ���ֽ�
							RS485_RX_buff[1] = RS485_START_AddrL; //��ʼ��ַ���ֽ�
							RS485_RX_buff[2] = RS485_Present_PDH; //�������ֽ�
							RS485_RX_buff[3] = RS485_Present_PDL; //�������ֽ�	
							RS485_RX_Correct_flag = 1;			  //���ݽ�����ȷ
							PostMessage (MSG_TYPE_RS485_RXDone, MSG_PRIORITY_LOW, RS485_RX_Correct_flag);
						}
						else
						{
							RS485_RX_Correct_flag = 0;
							PostMessage (MSG_TYPE_RS485_RXDone, MSG_PRIORITY_LOW, RS485_RX_Correct_flag);
						}
						RS485_RX_Count = 0;
						RS485_CRC16_RES = 0XFFFF;
					}
					break;

					case  Multi_Single_Register:
					if(RS485_RX_Count == 7)
					{
						RS485_RX_buff[4] = RS485_RX_Reg;		  //��Ҫ��д���ֽڸ���
					}
					else
					{
						RS485_RX_buff[5] = RS485_RX_Reg;		  //��Ҫ��д�ĵ�һ���ֽ���
												
					}
					RS485_CRC16_RES = RS485_CRC16(RS485_RX_Reg,RS485_CRC16_RES);
					break;
					default:
					RS485_RX_Count = 0;
					RS485_CRC16_RES = 0XFFFF;
					break;					
				}				
			}
			else
			{
				if(RS485_RX_Count <= RS485_RX_buff[4] + 7 )
				{
					RS485_RX_buff[RS485_RX_Count - 3] = RS485_RX_Reg;
					RS485_CRC16_RES = RS485_CRC16(RS485_RX_Reg,RS485_CRC16_RES);
				}
				else if(RS485_RX_Count == RS485_RX_buff[4] + 8 )
				{
					Check_RES = RS485_RX_Reg ;		          //У��λ�ĵ��ֽ�	
				}
				else if(RS485_RX_Count == RS485_RX_buff[4] + 9 )
				{
					Check_RES += RS485_RX_Reg*256; ;		  //У��λ	
					if(Check_RES == RS485_CRC16_RES) 
					{
						RS485_FUN_flag = RS485_FUN_num;       //��ȡ������
						RS485_RX_buff[0] = RS485_START_AddrH; //��ʼ��ַ���ֽ�
						RS485_RX_buff[1] = RS485_START_AddrL; //��ʼ��ַ���ֽ�
						RS485_RX_buff[2] = RS485_Present_PDH; //�������ֽ�
						RS485_RX_buff[3] = RS485_Present_PDL; //�������ֽ�	
						RS485_RX_Correct_flag = 1;			  //���ݽ�����ȷ
						PostMessage (MSG_TYPE_RS485_RXDone, MSG_PRIORITY_LOW, RS485_RX_Correct_flag);
					}
					else
					{
						RS485_RX_Correct_flag = 0;
						PostMessage (MSG_TYPE_RS485_RXDone, MSG_PRIORITY_LOW, RS485_RX_Correct_flag);
					}
					RS485_RX_Count = 0;
					RS485_CRC16_RES = 0XFFFF;
				}
			}					
		}
	}	
}

/***************************************************
�������ƣ�UART_IRQHandler
��ڲ�������
�������ܣ�UART�ж�
****************************************************/
void UART_IRQHandler()
{
	RS485_Rev(LPC_USART);
}

 void RS485_Init(void)
{
	uint32_t baudrate = 1200;
	if(baud == 0){ baudrate = 1200; }
	else if(baud == 1){ baudrate = 2400; }
	else if(baud == 2){ baudrate = 9600; }
	else if(baud == 3){ baudrate = 19200; }
	UART_Port_Init(baudrate);
}
 void RS485_Scan(void)
{	
}	

