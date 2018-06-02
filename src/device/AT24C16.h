#ifndef  __AT24C16_H
#define  __AT24C16_H

#include "chip.h"
#define AT24C16 1
#define AT24C02 0
#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000
#define I2C_DEFAULT_SPEED    SPEED_100KHZ

extern void i2c_app_init(I2C_ID_T id, int speed);
#if AT24C02
/*------------ע�⣺AT24C02�ĵ�ַΪ��0x00~0xFF��������Խ�磩------- */
extern uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr);	//ָ����ַ��ȡһ���ֽ�
extern void AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);	//ָ����ַд��һ���ֽ�
extern void AT24C02_Write_Page(uint8_t WriteAddr,uint8_t *Buffer);
extern void AT24C02_Write(uint8_t WriteAddr,uint8_t *Buffer,uint16_t Num);	//��ָ����ַ��ʼд��ָ�����ȵ�����
extern void AT24C02_Read(uint8_t ReadAddr,uint8_t *Buffer,uint16_t Num);   	//��ָ����ַ��ʼ����ָ�����ȵ�����
extern uint8_t AT24C02_Check(void);

#endif

#if AT24C16
extern uint8_t AT24C16_ReadOneByte(uint16_t ReadAddress);	//ָ����ַ��ȡһ���ֽ�
extern void AT24C16_WriteOneByte(uint16_t WriteAddress,uint8_t DataToWrite);	//ָ����ַд��һ���ֽ�
extern void AT24C16_Write_Page(uint16_t WriteAddress,uint8_t *Buffer);
extern void AT24C16_Write(uint16_t Address,uint8_t *Buffer,uint16_t Num);	//��ָ����ַ��ʼд��ָ�����ȵ�����
extern void AT24C16_Read(uint16_t ReadAddress,uint8_t *Buffer,uint16_t Num);   	//��ָ����ַ��ʼ����ָ�����ȵ�����
extern uint8_t AT24C16_Check(void);

#endif

#endif
/**************************end file******************************/
