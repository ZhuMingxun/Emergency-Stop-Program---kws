#ifndef __AMBE_H__
#define __AMBE_H__
#include "chip.h"

/*引脚定义(根据具体MCU修改)*/
//AMBE芯片 -> pin39
#define	AMBE_RST_PORT							2
#define	AMBE_RST_PINS							5
#define	AMBE_RST_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_5, (IOCON_FUNC0 | IOCON_MODE_INACT ));	


//AMBE芯片 -> pin65
#define	AMBE_PWR_PORT							1
#define	AMBE_PWR_PINS							1
#define	AMBE_PWR_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_1, (IOCON_FUNC1 | IOCON_MODE_INACT|IOCON_DIGMODE_EN));	
											

//AMBE芯片 -> pin64
#define AMBE_PRD_PORT							1
#define	AMBE_PRD_PINS							0
#define	AMBE_PRD_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_0, (IOCON_FUNC1 | IOCON_MODE_INACT));	
																	 

//AMBE芯片 -> pin60
#define	AMBE_PD0_PORT							0
#define	AMBE_PD0_PINS							11
#define	AMBE_PD0_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_11, (IOCON_FUNC1 | IOCON_MODE_INACT |IOCON_DIGMODE_EN));	
																	
//AMBE芯片 -> pin59
#define	AMBE_PD1_PORT							2
#define	AMBE_PD1_PINS							11
#define	AMBE_PD1_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_11, (IOCON_FUNC0 | IOCON_MODE_INACT ));	


//AMBE芯片 -> pin58
#define	AMBE_PD2_PORT							1
#define	AMBE_PD2_PINS							10
#define	AMBE_PD2_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_10, (IOCON_FUNC0 | IOCON_MODE_INACT|IOCON_DIGMODE_EN));	


//AMBE芯片 -> pin57
#define	AMBE_PD3_PORT							0
#define	AMBE_PD3_PINS							9
#define	AMBE_PD3_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC0 | IOCON_MODE_INACT ));	


//AMBE芯片 -> pin55
#define	AMBE_PD4_PORT							0
#define	AMBE_PD4_PINS							8
#define	AMBE_PD4_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC0 | IOCON_MODE_INACT));	


//AMBE芯片 -> pin54
#define	AMBE_PD5_PORT							2
#define	AMBE_PD5_PINS							2
#define	AMBE_PD5_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC0 | IOCON_MODE_INACT));	


//AMBE芯片 -> pin53
#define	AMBE_PD6_PORT							2
#define	AMBE_PD6_PINS							10
#define	AMBE_PD6_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_10, (IOCON_FUNC0 | IOCON_MODE_INACT));	


//AMBE芯片 -> pin52
#define	AMBE_PD7_PORT							2
#define	AMBE_PD7_PINS							9
#define	AMBE_PD7_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_9, (IOCON_FUNC0 | IOCON_MODE_INACT ));	


//AMBE芯片 -> pin47
#define	AMBE_DPE_PORT							0
#define	AMBE_DPE_PINS							7
#define	AMBE_DPE_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_7, (IOCON_FUNC0 | IOCON_MODE_INACT ));	


//AMBE芯片 -> pin46
#define	AMBE_EPR_PORT							0
#define	AMBE_EPR_PINS							6
#define	AMBE_EPR_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC0 | IOCON_MODE_INACT));	


/*GPIO移植函数(根据具体MCU修改)*/
 //#define	GPIO_CON()			LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);\
 //												LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16)
#define GPIO_OUT(a,b) 	Chip_GPIO_SetPinDIROutput(LPC_GPIO, a, b)
#define GPIO_IN(a,b)  	Chip_GPIO_SetPinDIRInput(LPC_GPIO, a, b)
#define GPIO_SET(a,b) 	Chip_GPIO_SetPinState(LPC_GPIO, a, b, 1)
#define GPIO_CLR(a,b) 	Chip_GPIO_SetPinState(LPC_GPIO, a, b, 0)
#define GPIO_GET(a,b) 	Chip_GPIO_GetPinState(LPC_GPIO, a, b)

/*芯片驱动函数*/
extern void  AMBEInit (void);				//AMBE-1000初始化
extern void  AMBEWrData(char*);			//AMBE-1000写入编码
extern char *AMBERdData(void);		  //AMBE-1000读取编码
extern char  AMBEGetWrStatus(void);	//AMBE-1000判断是否可以写入
extern char  AMBEGetRdStatus(void);	//AMBE-1000判断是否可以读取
extern void  AMBESetSine(void);			//AMBE-1000产生打点信号
extern void  AMBESetWake(void);			//AMBE-1000进入唤醒模式
extern void  AMBESetSleep(void);		//AMBE-1000进入休眠模式
extern char FrameRdData[34];
extern char FrameWrData[34];

#endif 
