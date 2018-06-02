#ifndef __SPEAK_H__
#define __SPEAK_H__
#include "chip.h"
#include "cari_config.h"
#if NEW_BS
/*引脚定义*/
//充电控制引脚1
#define	CHG1_PORT							3
#define	CHG1_PINS							3
#define	SPEAK_CHG1_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO3_3, (IOCON_FUNC0 | IOCON_MODE_INACT ));	

//充电控制引脚2
#define	CHG2_PORT							1
#define	CHG2_PINS							8
#define	SPEAK_CHG2_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_8, (IOCON_FUNC0 | IOCON_MODE_INACT ));	

//音量控制引脚1
#define	VOL1_PORT							1
#define	VOL1_PINS							9
#define	SPEAK_VOL1_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_9, (IOCON_FUNC0 | IOCON_MODE_INACT ));	

//音量控制引脚2
#define	VOL2_PORT							2
#define	VOL2_PINS							4
#define	SPEAK_VOL2_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_4, (IOCON_FUNC0 | IOCON_MODE_INACT));	

//讲话按键引脚
#define	KEY1_PORT							2
#define	KEY1_PINS							8
#define	SPEAK_KEY1_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_8, (IOCON_FUNC0 | IOCON_MODE_PULLUP  ));	//Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_8, (IOCON_FUNC0 | IOCON_MODE_INACT  ));	

//打点按键引脚
#define	KEY2_PORT							2
#define	KEY2_PINS							7
#define	SPEAK_KEY2_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP));//Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_7, (IOCON_FUNC0 | IOCON_MODE_INACT));	

//功放静音控制引脚
#define	MUTE_PORT							0
#define	MUTE_PINS							2
#define	SPEAK_MUTE_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC0 | IOCON_MODE_INACT  ));	

//LED控制引脚
#define	LED_PORT							2
#define	LED_PINS							1
#define	SPEAK_LED_IOCON()	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC0 | IOCON_MODE_INACT ));	

#define SGPIO_OUT(a,b) 	Chip_GPIO_SetPinDIROutput(LPC_GPIO, a, b)
#define SGPIO_IN(a,b)  	Chip_GPIO_SetPinDIRInput(LPC_GPIO, a, b)
#define SGPIO_SET(a,b) 	Chip_GPIO_SetPinState(LPC_GPIO, a, b, 1)
#define SGPIO_CLR(a,b) 	Chip_GPIO_SetPinState(LPC_GPIO, a, b, 0)
#define SGPIO_GET(a,b) 	Chip_GPIO_GetPinState(LPC_GPIO, a, b)

/*驱动函数*/
extern void  Led_ON (void);
extern void  Led_OFF(void);
extern void  Mute_ON (void);
extern void  Mute_OFF(void);
extern char  Get_Key1(void);
extern char  Get_Key2(void);
extern void  Set_Charge(unsigned char);
extern void  Set_Volume(unsigned char);
extern void  SBoard_Init(void);
extern void  Sine_Open(void);
extern void  Sine_Close(void);
#endif
#endif 
