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
/*------------注意：AT24C02的地址为（0x00~0xFF）（不可越界）------- */
extern uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr);	//指定地址读取一个字节
extern void AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);	//指定地址写入一个字节
extern void AT24C02_Write_Page(uint8_t WriteAddr,uint8_t *Buffer);
extern void AT24C02_Write(uint8_t WriteAddr,uint8_t *Buffer,uint16_t Num);	//从指定地址开始写入指定长度的数据
extern void AT24C02_Read(uint8_t ReadAddr,uint8_t *Buffer,uint16_t Num);   	//从指定地址开始读出指定长度的数据
extern uint8_t AT24C02_Check(void);

#endif

#if AT24C16
extern uint8_t AT24C16_ReadOneByte(uint16_t ReadAddress);	//指定地址读取一个字节
extern void AT24C16_WriteOneByte(uint16_t WriteAddress,uint8_t DataToWrite);	//指定地址写入一个字节
extern void AT24C16_Write_Page(uint16_t WriteAddress,uint8_t *Buffer);
extern void AT24C16_Write(uint16_t Address,uint8_t *Buffer,uint16_t Num);	//从指定地址开始写入指定长度的数据
extern void AT24C16_Read(uint16_t ReadAddress,uint8_t *Buffer,uint16_t Num);   	//从指定地址开始读出指定长度的数据
extern uint8_t AT24C16_Check(void);

#endif

#endif
/**************************end file******************************/
