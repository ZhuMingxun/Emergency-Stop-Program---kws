#include "AT24C16.h"
#define ROM	20170508 //�汾��
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

�������ܣ� ��ʼ��I2C���ߣ���main��������ã�

������ʽ��i2c_app_init(I2C0, 100000)����������Ϊ100K

*************************************************************/
void i2c_app_init(I2C_ID_T id, int speed)
{
	Init_I2C_PinMux();

	/* Initialize I2C */
	Chip_I2C_Init(id);    				//ʹ��I2Cʱ�ӣ���CLR�Ĵ���
	Chip_I2C_SetClockRate(id, speed);
	startMasterSetarp(LPC_I2C);			//ʹ��I2C //commet by eric 2017.5.13
}
/***************************************************/
/* �������ܣ����Ϳ�ʼ�ź�                          */
/***************************************************/
void I2C_Send_Start( )
{
	LPC_I2C->CONSET |= I2C_I2CONSET_STA;					// ʹ�ܷ��Ϳ�ʼ�ź�
	while(!(LPC_I2C->CONSET & I2C_I2CONSET_SI));   			// �ȴ���ʼ�źŷ������
	LPC_I2C->CONCLR = I2C_I2CONCLR_STAC | I2C_I2CONCLR_SIC; // �����ʼSTARTλ��SIλ 
}
/***************************************************/
/* �������ܣ�����ֹͣ�ź�                          */
/***************************************************/
void I2C_Stop( )
{
	LPC_I2C->CONSET |= I2C_I2CONSET_STO; 	// ����ֹͣ�ź�
	LPC_I2C->CONCLR = I2C_I2CONCLR_SIC;
	while( LPC_I2C->CONSET & I2C_I2CONSET_STO );
}

/***************************************************/
/* �������ܣ�I2C����һ�ֽ�����                     */
/* ��ڲ�����tx_data : Ҫ���͵��ֽ�                 */
/***************************************************/
void I2C_Send_Byte(uint8_t tx_data)
{
	LPC_I2C->DAT = tx_data;	  						// ���ֽ�д��DAT�Ĵ���
    while(!(LPC_I2C->CONSET & I2C_I2CONSET_SI));  // �ȴ����ݷ������
	LPC_I2C->CONCLR = I2C_I2CONCLR_SIC;  			 // ���SI��־
}

/***************************************************/
/* �������ܣ�I2C����һ�ֽ�����                     */
/* ��ڲ�����rebyte : Ҫ���յ��ֽ�                 */
/***************************************************/
uint8_t I2C_Recieve_Byte(void)
{
	uint8_t rebyte;

	while(!(LPC_I2C->CONSET & I2C_I2CONSET_SI));  // �ȴ������������
	rebyte = (uint8_t)LPC_I2C->DAT;				// �ѽ��յ������ݸ���rebyte
	LPC_I2C->CONCLR = I2C_I2CONCLR_SIC;  		 // ���SI��־

	return rebyte;
}

/*������ʱ��Ϊ22M�趨�Ķ�ʱ�������Ͼ�ȷ*/
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
/* �������ܣ���AT24C02��дһ���ֽ�����       */
/* ��ڲ�����WriteAddr:��Ҫд���Ŀ���ַ    */
/*           DataToWrite:��Ҫд����ֽ�����  */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
void AT24C02_WriteOneByte(uint8_t WriteAddr, uint8_t DataToWrite)
{		
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0);   
	I2C_Send_Byte(WriteAddr);		 //���͵�ַ 										  		   
	I2C_Send_Byte(DataToWrite);     //�����ֽ�							   		    	   
	I2C_Stop();						//����һ��ֹͣ���� 
	delay_24c16();					// ��ʱ����5ms �ȴ�24C02д����� 
}

/*********************************************/
/* �������ܣ���AT24C02�ж�һ���ֽ�����       */
/* ���ڲ�����temp :����������                */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t temp=0;
	
	I2C_Send_Start();
	I2C_Send_Byte(0xA0); 	 
	I2C_Send_Byte(ReadAddr);  	 //���͵�ַ 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);			   
	temp=I2C_Recieve_Byte();	   
	I2C_Stop();					 //����һ��ֹͣ����
		    
	return temp;
}

/*********************************************/
/* �������ܣ���AT24C02�ж�����ֽ�����       */
/* ��ڲ�����ReadAddr:��Ҫ�����ݵ�Ŀ���ַ   */
/*           *Buffer:�����������ŵ��������  */
/*           Num��Ҫ���������ֽڸ���         */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
void AT24C02_Read(uint8_t ReadAddr,uint8_t *Buffer,uint16_t Num)
{
	I2C_Send_Start();
	I2C_Send_Byte(0xA0); 	 
	I2C_Send_Byte(ReadAddr);  		 //���͵�ַ 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);
	LPC_I2C->CONSET |=(1<<2);
	while(--Num)
	{
		*Buffer++=I2C_Recieve_Byte();
	}
	LPC_I2C->CONCLR = (1<<2);
	*Buffer++=I2C_Recieve_Byte();	
	I2C_Stop();						//����һ��ֹͣ����
}  

/*********************************************/
/* �������ܣ���AT24C02��д8�ֽ�����(һҳ)    */
/* ��ڲ�����WriteAddr:��Ҫд���ݵ�Ŀ���ַ  */
/*           *Buffer:����������е�����д��  */
/*           Num��Ҫд�������ֽڸ���         */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
void AT24C02_Write_Page(uint8_t WriteAddr,uint8_t *Buffer)
{
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0);   
	I2C_Send_Byte(WriteAddr);	  //���͵�ַ 										  		   
	I2C_Send_Byte(*Buffer++);     //�����ֽ�1
	I2C_Send_Byte(*Buffer++);     //�����ֽ�2
	I2C_Send_Byte(*Buffer++);     //�����ֽ�3
	I2C_Send_Byte(*Buffer++);     //�����ֽ�4
	I2C_Send_Byte(*Buffer++);     //�����ֽ�5
	I2C_Send_Byte(*Buffer++);     //�����ֽ�6
	I2C_Send_Byte(*Buffer++);     //�����ֽ�7
	I2C_Send_Byte(*Buffer++);     //�����ֽ�8
	I2C_Stop();					  //����һ��ֹͣ���� 
	delay_24c16();				  // ��ʱ����5ms �ȴ�24C02д����� 
}

/***********************************************/
/* �������ܣ����AT24C02�Ƿ����               */
/* ���ڲ�����0������                           */
/*           1��������                         */
/***********************************************/
uint8_t AT24C02_Check(void)
{
	uint8_t temp;
	temp = AT24C02_ReadOneByte(0x00);	 // ���ֽ�		   
	if(temp==0x88)return 0;		   
	else								//�ų���һ�γ�ʼ�������
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
/* �������ܣ���AT24C16��дһ���ֽ�����       */
/* ��ڲ�����WriteAddr:��Ҫд���Ŀ���ַ    */
/*           DataToWrite:��Ҫд����ֽ�����  */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
void AT24C16_WriteOneByte(uint16_t Address, uint8_t DataToWrite)
{	
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0+((Address/256)<<1));   
	I2C_Send_Byte(Address%256);		 //���͵�ַ 										  		   
	I2C_Send_Byte(DataToWrite);     //�����ֽ�							   		    	   
	I2C_Stop();						//����һ��ֹͣ���� 
	delay_24c16();					// ��ʱ����5ms �ȴ�24C02д����� 
}

/*********************************************/
/* �������ܣ���AT24C16�ж�һ���ֽ�����       */
/* ���ڲ�����temp :����������                */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
uint8_t AT24C16_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;
	
	I2C_Send_Start();
	I2C_Send_Byte(0xA0+((ReadAddr/256)<<1)); 	 
	I2C_Send_Byte(ReadAddr%256);  	 //���͵�ַ 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);			   
	temp=I2C_Recieve_Byte();	   
	I2C_Stop();					 //����һ��ֹͣ����
		    
	return temp;
}

/*********************************************/
/* �������ܣ���AT24C16�ж�����ֽ�����       */
/* ��ڲ�����ReadAddr:��Ҫ�����ݵ�Ŀ���ַ   */
/*           *Buffer:�����������ŵ��������  */
/*           Num��Ҫ���������ֽڸ���         */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
void AT24C16_Read(uint16_t ReadAddr,uint8_t *Buffer,uint16_t Num)
{
	I2C_Send_Start();
	I2C_Send_Byte(0xA0); 	 
	I2C_Send_Byte(ReadAddr);  		 //���͵�ַ 
	I2C_Send_Start();		
	I2C_Send_Byte(0xA1);
	LPC_I2C->CONSET |=(1<<2);
	while(--Num)
	{
		*Buffer++=I2C_Recieve_Byte();
	}
	LPC_I2C->CONCLR = (1<<2);
	*Buffer++=I2C_Recieve_Byte();	
	I2C_Stop();						//����һ��ֹͣ����
}  

/*********************************************/
/* �������ܣ���AT24C16��д8�ֽ�����(һҳ)    */
/* ��ڲ�����WriteAddr:��Ҫд���ݵ�Ŀ���ַ  */
/*           *Buffer:����������е�����д��  */
/*           Num��Ҫд�������ֽڸ���         */
/* ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��    */
/*********************************************/
void AT24C16_Write_Page(uint16_t WriteAddr,uint8_t *Buffer)
{
	I2C_Send_Start();	
	I2C_Send_Byte(0xA0);   
	I2C_Send_Byte(WriteAddr);	  //���͵�ַ 										  		   
	I2C_Send_Byte(*Buffer++);     //�����ֽ�1
	I2C_Send_Byte(*Buffer++);     //�����ֽ�2
	I2C_Send_Byte(*Buffer++);     //�����ֽ�3
	I2C_Send_Byte(*Buffer++);     //�����ֽ�4
	I2C_Send_Byte(*Buffer++);     //�����ֽ�5
	I2C_Send_Byte(*Buffer++);     //�����ֽ�6
	I2C_Send_Byte(*Buffer++);     //�����ֽ�7
	I2C_Send_Byte(*Buffer++);     //�����ֽ�8
	I2C_Stop();					  //����һ��ֹͣ���� 
	delay_24c16();				  // ��ʱ����5ms �ȴ�24C02д����� 
}

/***********************************************/
/* �������ܣ����AT24C16�Ƿ����               */
/* ���ڲ�����0������                           */
/*           1��������                         */
/***********************************************/
uint8_t AT24C16_Check(void)
{
	uint8_t temp;
	temp = AT24C16_ReadOneByte(0x00);	 // ���ֽ�		   
	if(temp==0x88)return 0;		   
	else								//�ų���һ�γ�ʼ�������
	{
		AT24C16_WriteOneByte(0x00,0x88);
		temp=AT24C16_ReadOneByte(0x00);	  
		if(temp==0X88)
		{
			uint32_t release_no;
			AT24C16_WriteOneByte(0x01,0x03);
			AT24C16_WriteOneByte(0x02,0x00);
			//BYTE 2001 ����汾�ŵ�λ
			//BYTE 2002 ����汾�Ÿ�λ
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
