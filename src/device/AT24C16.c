#include "AT24C16.h"
#define ROM	20170508 //版本号
void Init_I2C_PinMux(void)
{

	Chip_SYSCTL_PeriphReset(RESET_I2C0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_4, IOCON_FUNC1 );
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_5, IOCON_FUNC1 );

}

void startMasterSetarp(LPC_I2C_T *pI2C)
{
	/* Reset STA, STO, SI */
	pI2C->CONCLR = I2C_CON_SI | I2C_CON_STO | I2C_CON_STA;

	/* Enter to Master Transmitter mode */
	pI2C->CONSET = I2C_CON_I2EN ;
}

/***************************************************************

函数功能： 初始化I2C总线，在main函数里调用，

调用形式：i2c_app_init(I2C0, 100000)，总线速率为100K

*************************************************************/
void i2c_app_init(I2C_ID_T id, int speed)
{
	Init_I2C_PinMux();

	/* Initialize I2C */
	Chip_I2C_Init(id);    				//使能I2C时钟，清CLR寄存器
	Chip_I2C_SetClockRate(id, speed);
	startMasterSetarp(LPC_I2C);			//使能I2C //commet by eric 2017.5.13
}
/***************************************************/
/* 函数功能：发送开始信号                          */
/***************************************************/
void I2C_Send_Start( )
{
	LPC_I2C->CONSET |= I2C_I2CONSET_STA;					// 使能发送开始信号
	while(!(LPC_I2C->CONSET & I2C_I2CONSET_SI));   			// 等待开始信号发送完成
	LPC_I2C->CONCLR = I2C_I2CONCLR_STAC | I2C_I2CONCLR_SIC; // 清除开始START位和SI位 
}
/***************************************************/
/* 函数功能：发送停止信号                          */
/***************************************************/
void I2C_Stop( )
{
	LPC_I2C->CONSET |= I2C_I2CONSET_STO; 	// 发送停止信号
	LPC_I2C->CONCLR = I2C_I2CONCLR_SIC;
	while( LPC_I2C->CONSET & I2C_I2CONSET_STO );
}

/***************************************************/
/* 函数功能：I2C发送一字节数据                     */
/* 入口参数：tx_data : 要发送的字节                 */
/***************************************************/
void I2C_Send_Byte(uint8_t tx_data)
{
	LPC_I2C->DAT = tx_data;	  						// 把字节写入DAT寄存器
    while(!(LPC_I2C->CONSET & I2C_I2CONSET_SI));  // 等待数据发送完成
	LPC_I2C->CONCLR = I2C_I2CONCLR_SIC;  			 // 清除SI标志
}

/***************************************************/
/* 函数功能：I2C接收一字节数据                     */
/* 入口参数：rebyte : 要接收的字节                 */
/***************************************************/
uint8_t I2C_Recieve_Byte(void)
{
	uint8_t rebyte;

	while(!(LPC_I2C->CONSET & I2C_I2CONSET_SI));  // 等待接收数据完成
	rebyte = (uint8_t)LPC_I2C->DAT;				// 把接收到的数据给了rebyte
	LPC_I2C->CONCLR = I2C_I2CONCLR_SIC;  		 // 清除SI标志

	return rebyte;
}

/*根据主时钟为22M设定的定时函数，较精确*/
volatile void delay_24c16(void)
{
	uint16_t i,j;
	
	for(j=0;j<25;j++)
	{
		for(i=0;i<900;i++);
	}
}

#if AT24C02
/*********************************************/
/* 函数功能：给AT24C02中写一个字节数据       */
/* 入口参数：WriteAddr:将要写入的目标地址    */
/*           DataToWrite:将要写入的字节数据  */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
void AT24C02_WriteOneByte(uint8_t WriteAddr, uint8_t DataToWrite)
{		
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0);   
	I2C_Send_Byte(WriteAddr);		 //发送地址 										  		   
	I2C_Send_Byte(DataToWrite);     //发送字节							   		    	   
	I2C_Stop();						//产生一个停止条件 
	delay_24c16();					// 延时大于5ms 等待24C02写入完成 
}

/*********************************************/
/* 函数功能：从AT24C02中读一个字节数据       */
/* 出口参数：temp :读出的数据                */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t temp=0;
	
	I2C_Send_Start();
	I2C_Send_Byte(0xA0); 	 
	I2C_Send_Byte(ReadAddr);  	 //发送地址 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);			   
	temp=I2C_Recieve_Byte();	   
	I2C_Stop();					 //产生一个停止条件
		    
	return temp;
}

/*********************************************/
/* 函数功能：从AT24C02中读多个字节数据       */
/* 入口参数：ReadAddr:将要读数据的目标地址   */
/*           *Buffer:读出数据来放到这个数组  */
/*           Num：要读的数据字节个数         */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
void AT24C02_Read(uint8_t ReadAddr,uint8_t *Buffer,uint16_t Num)
{
	I2C_Send_Start();
	I2C_Send_Byte(0xA0); 	 
	I2C_Send_Byte(ReadAddr);  		 //发送地址 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);
	LPC_I2C->CONSET |=(1<<2);
	while(--Num)
	{
		*Buffer++=I2C_Recieve_Byte();
	}
	LPC_I2C->CONCLR = (1<<2);
	*Buffer++=I2C_Recieve_Byte();	
	I2C_Stop();						//产生一个停止条件
}  

/*********************************************/
/* 函数功能：给AT24C02中写8字节数据(一页)    */
/* 入口参数：WriteAddr:将要写数据的目标地址  */
/*           *Buffer:把这个数组中的数据写入  */
/*           Num：要写的数据字节个数         */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
void AT24C02_Write_Page(uint8_t WriteAddr,uint8_t *Buffer)
{
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0);   
	I2C_Send_Byte(WriteAddr);	  //发送地址 										  		   
	I2C_Send_Byte(*Buffer++);     //发送字节1
	I2C_Send_Byte(*Buffer++);     //发送字节2
	I2C_Send_Byte(*Buffer++);     //发送字节3
	I2C_Send_Byte(*Buffer++);     //发送字节4
	I2C_Send_Byte(*Buffer++);     //发送字节5
	I2C_Send_Byte(*Buffer++);     //发送字节6
	I2C_Send_Byte(*Buffer++);     //发送字节7
	I2C_Send_Byte(*Buffer++);     //发送字节8
	I2C_Stop();					  //产生一个停止条件 
	delay_24c16();				  // 延时大于5ms 等待24C02写入完成 
}

/***********************************************/
/* 函数功能：检测AT24C02是否存在               */
/* 出口参数：0：存在                           */
/*           1：不存在                         */
/***********************************************/
uint8_t AT24C02_Check(void)
{
	uint8_t temp;
	temp = AT24C02_ReadOneByte(0x00);	 // 读字节		   
	if(temp==0x88)return 0;		   
	else								//排除第一次初始化的情况
	{
		AT24C02_WriteOneByte(0x00,0x88);
		temp=AT24C02_ReadOneByte(0x00);	  
		if(temp==0X88)return 0;
	}
	return 1;											  
}
#endif
#if AT24C16
/*********************************************/
/* 函数功能：给AT24C16中写一个字节数据       */
/* 入口参数：WriteAddr:将要写入的目标地址    */
/*           DataToWrite:将要写入的字节数据  */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
void AT24C16_WriteOneByte(uint16_t Address, uint8_t DataToWrite)
{	
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0+((Address/256)<<1));   
	I2C_Send_Byte(Address%256);		 //发送地址 										  		   
	I2C_Send_Byte(DataToWrite);     //发送字节							   		    	   
	I2C_Stop();						//产生一个停止条件 
	delay_24c16();					// 延时大于5ms 等待24C02写入完成 
}

/*********************************************/
/* 函数功能：从AT24C16中读一个字节数据       */
/* 出口参数：temp :读出的数据                */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
uint8_t AT24C16_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;
	
	I2C_Send_Start();
	I2C_Send_Byte(0xA0+((ReadAddr/256)<<1)); 	 
	I2C_Send_Byte(ReadAddr%256);  	 //发送地址 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);			   
	temp=I2C_Recieve_Byte();	   
	I2C_Stop();					 //产生一个停止条件
		    
	return temp;
}

/*********************************************/
/* 函数功能：从AT24C16中读多个字节数据       */
/* 入口参数：ReadAddr:将要读数据的目标地址   */
/*           *Buffer:读出数据来放到这个数组  */
/*           Num：要读的数据字节个数         */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
void AT24C16_Read(uint16_t ReadAddr,uint8_t *Buffer,uint16_t Num)
{
	I2C_Send_Start();
	I2C_Send_Byte(0xA0); 	 
	I2C_Send_Byte(ReadAddr);  		 //发送地址 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);
	LPC_I2C->CONSET |=(1<<2);
	while(--Num)
	{
		*Buffer++=I2C_Recieve_Byte();
	}
	LPC_I2C->CONCLR = (1<<2);
	*Buffer++=I2C_Recieve_Byte();	
	I2C_Stop();						//产生一个停止条件
}  

/*********************************************/
/* 函数功能：给AT24C16中写8字节数据(一页)    */
/* 入口参数：WriteAddr:将要写数据的目标地址  */
/*           *Buffer:把这个数组中的数据写入  */
/*           Num：要写的数据字节个数         */
/* 注意：AT24C02的地址为（0x00~0xFF）    */
/*********************************************/
void AT24C16_Write_Page(uint16_t WriteAddr,uint8_t *Buffer)
{
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0);   
	I2C_Send_Byte(WriteAddr);	  //发送地址 										  		   
	I2C_Send_Byte(*Buffer++);     //发送字节1
	I2C_Send_Byte(*Buffer++);     //发送字节2
	I2C_Send_Byte(*Buffer++);     //发送字节3
	I2C_Send_Byte(*Buffer++);     //发送字节4
	I2C_Send_Byte(*Buffer++);     //发送字节5
	I2C_Send_Byte(*Buffer++);     //发送字节6
	I2C_Send_Byte(*Buffer++);     //发送字节7
	I2C_Send_Byte(*Buffer++);     //发送字节8
	I2C_Stop();					  //产生一个停止条件 
	delay_24c16();				  // 延时大于5ms 等待24C02写入完成 
}

/***********************************************/
/* 函数功能：检测AT24C16是否存在               */
/* 出口参数：0：存在                           */
/*           1：不存在                         */
/***********************************************/
uint8_t AT24C16_Check(void)
{
	uint8_t temp;
	temp = AT24C16_ReadOneByte(0x00);	 // 读字节		   
	if(temp==0x88)return 0;		   
	else								//排除第一次初始化的情况
	{
		AT24C16_WriteOneByte(0x00,0x88);
		temp=AT24C16_ReadOneByte(0x00);	  
		if(temp==0X88)
		{
			uint32_t release_no;
			AT24C16_WriteOneByte(0x01,0x03);
			AT24C16_WriteOneByte(0x02,0x00);
			//BYTE 2001 软件版本号低位
			//BYTE 2002 软件版本号高位
			release_no=ROM;
			AT24C16_WriteOneByte(2001,release_no%10000);
			AT24C16_WriteOneByte(2002,release_no/
			10000);
			
		}
		return 0;
	}
	//return 1;											  
}
#endif
/*******************end file***********************/
