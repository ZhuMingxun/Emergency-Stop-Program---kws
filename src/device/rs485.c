
///////////////////////////////////////////////////////////////////////////
/// @file  led.c
/// @brief 组件源程序
/// 
/// 组件源程序包括功能：节拍中断中采集PIO3_3,通过消息传递函数传出
/// 在message.h中定义消息类型，#define MSG_TYPE_XXX             2
/// <b>Copyright （c） 
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////
/// includes
/////////////////////////////////////////////////
#include "chip.h"
#include "rs485.h"
#include "Message.h"

/////////////////////////////////////////////////////////////////////
/// 函数声明
////////////////////////////////////////////////////////////////////
#define Read_Holding_Register      3	/*0x03功能码宏定义*/
#define Preset_Single_Register     6	/*0x06功能码宏定义*/
#define Multi_Single_Register      16	/*0x10功能码宏定义*/

#define RS485_TX 	Chip_GPIO_SetPinState(LPC_GPIO, 1, 0, 0)
#define RS485_RX 	Chip_GPIO_SetPinState(LPC_GPIO, 1, 0, 1)

uint8_t  baud = 0;
uint8_t  RS485_Addr;					//分机地址
uint8_t  RS485_TX_buff[515];		    //正确接收后回复数据数组,最大515个字节，根据需要改小，寄存器个数0xff
uint8_t  RS485_RX_buff[264];			//接收数据暂存数组，最大264个字节，寄存器个数0xff
uint8_t  RS485_TX_Length;               //发送数据的长度
uint8_t  RS485_FUN_flag;				//RS485功能码标志
uint8_t  RS485_START_AddrH;				//起始地址高字节
uint8_t  RS485_START_AddrL;				//起始地址低字节
uint8_t  RS485_Present_PDH;				//预置个数高字节
uint8_t  RS485_Present_PDL;				//预置个数低字节
uint8_t  RS485_RX_Correct_flag;			//接收正确标志
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
函数名称：UART_Port_Init
入口参数：baudrate:波特率
函数功能：UART初始化，9600,8N1
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
函数名称：RS485_CRC16
入口参数：rec_dat:当前接收到的数据；crc16_result:前校验结果
函数功能：实现CRC16校验功能
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
函数名称：RS485_Send
入口参数：UART句柄，数组/数据地址，长度
函数功能：UART发送数据
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
函数名称：MessSend
入口参数：无
函数功能：数据装载
****************************************************/
void MessSend()
{
	uint8_t  x;								   //循环变量
	uint16_t Temp_Lth;						   //字节长度控制
	uint16_t Addr_offset;					   //起始地址
	uint16_t Temp_Con;						   //用于存放读取字节个数
	uint16_t RS485_TX_CRC16_RES = 0xffff;      //校验值

	Chip_UART_IntDisable(LPC_USART, UART_IER_RBRINT);
	RS485_TX;								   //置为发送状态
	if(RS485_FUN_flag != 0)
	{
		switch(RS485_FUN_flag)
		{
////////////////读单个寄存/////////////////////////////////////////////////////////
			case Read_Holding_Register:			
		   	Temp_Lth = ( RS485_RX_buff[2]*256 + RS485_RX_buff[3])*2; //计算读取字节个数
			if(Temp_Lth > 254) Temp_Con = 254;					     //寄存器个数控制
			else Temp_Con = Temp_Lth;
			Addr_offset = 2*(RS485_RX_buff[0]*256 + RS485_RX_buff[1]);	 //计算所读数据的起始地址
			RS485_TX_buff[0] = RS485_Addr;						     //分机地址
			RS485_TX_buff[1] = RS485_FUN_flag;					     //功能码
			RS485_TX_buff[2] = Temp_Con;						     //字节个数
			for(x = 0; x < Temp_Con; x++)						   
			{
				RS485_TX_buff[3 + x] = RS485_Store_Data[Addr_offset + x];                       //此处要装载数据
			}
			for(x = 0; x < 3+Temp_Con; x++)					         //校验值
			{
				RS485_TX_CRC16_RES = RS485_CRC16(RS485_TX_buff[x],RS485_TX_CRC16_RES);	
			}
			RS485_TX_buff[3+Temp_Con] = RS485_TX_CRC16_RES;
			RS485_TX_buff[4+Temp_Con] = RS485_TX_CRC16_RES>>8;
			RS485_TX_Length = 5+Temp_Con;	                         //数据长度
			RS485_FUN_flag = 0;
			break;
////////////////////////////////////////////////////////////////////////////////////////////
/////////////////预置单个寄存器///////////////////////////////////////////////////////////
			case Preset_Single_Register:
			Addr_offset =2*(RS485_RX_buff[0]*256 + RS485_RX_buff[1]);	 //计算所写数据的起始地址							 
			RS485_TX_buff[0] = RS485_Addr;							 //装载地址
			RS485_TX_buff[1] = RS485_FUN_flag;						 //装载功能码
			for(x = 0; x < 4; x++)
			{
				RS485_TX_buff[2+x] = RS485_RX_buff[x];				 //装载回复数据
			}
			for(x = 0;x < 2; x++)
			{
				RS485_Store_Data[Addr_offset + x] =  RS485_RX_buff[2+x]; //存储数据
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
//////////////////////////预置多个寄存器//////////////////////////////
			case  Multi_Single_Register:
			Addr_offset = RS485_RX_buff[0]*256 + RS485_RX_buff[1];	 //计算所写数据的起始地址							 
			RS485_TX_buff[0] = RS485_Addr;							 //装载地址
			RS485_TX_buff[1] = RS485_FUN_flag;						 //装载功能码
			for(x = 0; x < 4; x++)
			{
				RS485_TX_buff[2+x] = RS485_RX_buff[x];				 //装载回复数据
			}
			for(x = 0;x < RS485_RX_buff[4]; x++)
			{
				RS485_Store_Data[Addr_offset + x] =  RS485_RX_buff[5+x]; //存储数据
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
函数名称：RS485_Rev
入口参数：UART句柄
函数功能：数据接收
****************************************************/
void RS485_Rev(LPC_USART_T *pUART)
{
	static uint16_t Check_RES;					  //校验结果
//	static uint8_t  RS485_RX_Count;				  //RS485接收字节计数器
	static uint8_t  RS485_FUN_num;				  //RS485功能码
//	static uint16_t RS485_CRC16_RES = 0XFFFF;     //CRC校验初始结果
	uint8_t RS485_RX_Reg;

	while ((Chip_UART_ReadIntIDReg(pUART) & 0x01) == 0 ) 
	{
		if( (Chip_UART_ReadIntIDReg(pUART) & 0x0E) == 0x04 )
		{
			RS485_RX_Reg = 	Chip_UART_ReadByte(pUART);
			RS485_RX_Count++;
			
			RS485_DOG = 30;
			if(RS485_RX_Count == 1)				  //接收到第一个字节
			{
				if(RS485_RX_Reg == RS485_Addr)	  //第一个是地址数据
				{
					RS485_CRC16_RES = RS485_CRC16(RS485_RX_Reg,RS485_CRC16_RES);	
				}
				else 
				{
					RS485_RX_Count = 0;	
					RS485_CRC16_RES = 0XFFFF;
				}
			}
			else if(RS485_RX_Count == 2)		 //第二个字节是功能码
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
						RS485_START_AddrH = RS485_RX_Reg ;		//存储高字节地址
					}
					else
					{
					 	RS485_START_AddrL = RS485_RX_Reg ;		//存储低字节地址
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
						RS485_Present_PDH = RS485_RX_Reg ;		//存储高字节个数
					}
					else
					{
					 	RS485_Present_PDL = RS485_RX_Reg ;		//存储低字节个数
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
						Check_RES = RS485_RX_Reg ;		          //校验位的低字节
					}
					else
					{
					 	Check_RES += RS485_RX_Reg*256;            //校验结果
						if(Check_RES == RS485_CRC16_RES) 
						{
							RS485_FUN_flag = RS485_FUN_num;       //获取功能码
							RS485_RX_buff[0] = RS485_START_AddrH; //起始地址高字节
							RS485_RX_buff[1] = RS485_START_AddrL; //起始地址低字节
							RS485_RX_buff[2] = RS485_Present_PDH; //个数高字节
							RS485_RX_buff[3] = RS485_Present_PDL; //个数低字节	
							RS485_RX_Correct_flag = 1;			  //数据接收正确
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
						RS485_RX_buff[4] = RS485_RX_Reg;		  //所要改写的字节个数
					}
					else
					{
						RS485_RX_buff[5] = RS485_RX_Reg;		  //所要改写的第一个字节数
												
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
					Check_RES = RS485_RX_Reg ;		          //校验位的低字节	
				}
				else if(RS485_RX_Count == RS485_RX_buff[4] + 9 )
				{
					Check_RES += RS485_RX_Reg*256; ;		  //校验位	
					if(Check_RES == RS485_CRC16_RES) 
					{
						RS485_FUN_flag = RS485_FUN_num;       //获取功能码
						RS485_RX_buff[0] = RS485_START_AddrH; //起始地址高字节
						RS485_RX_buff[1] = RS485_START_AddrL; //起始地址低字节
						RS485_RX_buff[2] = RS485_Present_PDH; //个数高字节
						RS485_RX_buff[3] = RS485_Present_PDL; //个数低字节	
						RS485_RX_Correct_flag = 1;			  //数据接收正确
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
函数名称：UART_IRQHandler
入口参数：无
函数功能：UART中断
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

